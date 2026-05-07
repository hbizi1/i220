#include "cache-sim.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
usage(const char *program, const char *msg)
{
  fprintf(stderr, "%susage: %s [-r lru|mru|rand] [-s seed] [-q] m-s-b-E\n"
          "where m-s-b-E specified cache parameters:\n"
          "  m: total # of bits used to address memory\n"
          "  s: # of bits in address used to specify set index\n"
          "  b: # of bits in address used to specify offset in cache block\n"
          "  E: # of cache lines per set\n"
          "  must have all non-negative and 2 <= b and b + s < m\n",
          msg, program);
    exit(1);
}

typedef struct {
  const char *name;
  Replacement replacement;
} ReplacementName;

static ReplacementName REPLACEMENTS[] = {
  { "lru", LRU_R },
  { "mru", MRU_R },
  { "rand", RANDOM_R },
};

/** Translate from name to Replacement enum.  Return < 0 on error */
static int
get_replacement(const char *name) {
  for (int i = 0; i < sizeof(REPLACEMENTS)/sizeof(REPLACEMENTS[0]); i++) {
    if (strcmp(name, REPLACEMENTS[i].name) == 0) {
      return REPLACEMENTS[i].replacement;
    }
  }
  return -1;
}

/** Somewhat non-elegant allocation here to force new_cache_sim() to
 *  make copies of *params.  Sets *n_mem_addr_bits which is used for
 *  verbose formatting in do_cache_sim().  Returns NULL on error.
 */
static CacheSim *
make_cache_sim(const char *params_spec, Replacement replacement,
               unsigned *n_mem_addr_bits)
{
  CacheParams params;
  params.replacement = replacement;
  if (sscanf(params_spec, "%u-%u-%u-%u",
             &params.n_mem_addr_bits, &params.n_set_index_bits,
             &params.n_blk_offset_bits, &params.n_lines_per_set) != 4) {
    return NULL;
  }
  *n_mem_addr_bits = params.n_mem_addr_bits;
  bool is_sum_ok =
    params.n_set_index_bits + params.n_blk_offset_bits < params.n_mem_addr_bits;
  return (is_sum_ok && 2 <= params.n_blk_offset_bits)
         ? new_cache_sim(&params)
         : NULL;
}

static void
out_cache_stats(unsigned long stats[], unsigned long nTotal, FILE *out)
{
  enum { W = 30 };
  for (int i = 0; i < CACHE_N_STATUS + 1; i++) {
    switch (i) {
    case CACHE_HIT:
      fprintf(out, "%-*s", W, "# hits:");
      break;
    case CACHE_MISS_WITHOUT_REPLACE:
      fprintf(out,  "%-*s", W, "# misses without replace:");
      break;
    case CACHE_MISS_WITH_REPLACE:
      fprintf(out,  "%-*s", W, "# misses with replace:");
      break;
    case CACHE_N_STATUS:
      fprintf(out,  "%-*s", W, "# dirty writes:");
      break;
    }
    fprintf(out, " %lu/%lu (%.2f%%)\n", stats[i], nTotal,
          (nTotal == 0) ? 0 : stats[i] * 100.0/nTotal);
  }
}

//must be in sync with CACHE_STATUS enum
static const char *STATUS_STRS[] = {
  "h", "m", "M"
};

typedef struct {
  enum { OK_READ, SKIP_READ, ERR_READ } status;
  MemAddr addr;
  char rw;
} AddrRead;

static AddrRead
parse_access(const char *line)
{
  AddrRead read;
  char *p;
  for (p = (char *)line; isspace(*p); p++) {
    //skip space
  }
  if (*p == '#' || *p == '\0') { read.status = SKIP_READ; return read; }
  if (*p != '0' || (p[1] != 'x' && p[1] != 'X')) {
    read.status = ERR_READ; return read;
  }
  read.addr = strtol(line, &p, 0);
  if (p == line) { read.status = ERR_READ; return read; }
  char rw = 'r';
  sscanf(p, " %c", &rw);
  if (rw == '#') { read.rw = 'r'; return read; }
  if (rw != 'r' && rw != 'w') { read.status = ERR_READ; return read; };
  read.status = OK_READ;
  read.rw = rw;
  return read;
}

static void
do_cache_sim(CacheSim *cache, bool is_quiet,
             unsigned n_mem_addr_bits, FILE *in, FILE *out)
{
  unsigned long stats[CACHE_N_STATUS + 1] = { 0UL };
  unsigned addr_width = (n_mem_addr_bits + 3)/4;
  unsigned long n_total = 0UL;
  char *line = NULL;
  size_t line_size = 0;
  while (1) {
    if (getline(&line, &line_size, stdin) < 0) break;
    AddrRead read = parse_access(line);
    if (read.status == ERR_READ) {
      fprintf(stderr, "bad input; must be \"0[xX]HEX [r|w|\" (default 'r')\n");
      continue;
    }
    else if (read.status == SKIP_READ) {
      continue;
    }
    bool is_write = read.rw == 'w';
    n_total++;
    CacheResult result = cache_sim_result(cache, read.addr, is_write);
    stats[result.status]++;
    if (result.status == CACHE_MISS_WITH_REPLACE && result.is_dirty) {
      stats[CACHE_N_STATUS]++;
    }
    if (!is_quiet) {
      fprintf(out, "0x%0*lx %c: %s", addr_width, read.addr, read.rw,
              STATUS_STRS[result.status]);
      if (result.status == CACHE_MISS_WITH_REPLACE) {
        fprintf(out, " 0x%0*lx%s", addr_width, result.replace_addr,
                result.is_dirty ? " w" : "");
      }
      fprintf(out, "\n");
    }
  } // while (1)
  free(line);
  out_cache_stats(stats, n_total, out);
}

int
main(int argc, const char *argv[])
{
  const char *program = argv[0];
  if (argc <= 1) usage(program, "");
  bool is_quiet = false;
  int replacement = LRU_R;
  int seed = 0;
  int i;
  for (i = 1; i < argc && argv[i][0] == '-'; i++) {
    if (strcmp(argv[i], "-q") == 0) {
      is_quiet = true;
    }
    else if (strcmp(argv[i], "-r") == 0) {
      if (i >= argc - 1) {
        usage(program, "-r requires lru|mru|rand additional argument\n");
      }
      replacement = get_replacement(argv[++i]);
      if (replacement < 0) {
        usage(program, "replacement must be lru|mru|rand\n");
      }
    }
    else if (strcmp(argv[i], "-s") == 0) {
      if (i >= argc - 1) {
        usage(program, "-s requires seed additional argument\n");
      }
      char *p;
      seed = strtol(argv[++i], &p, 10);
      if (seed < 0 || *p != '\0') {
        usage(program, "seed must be a non-negative integer\n");
      }
    }
    else {
      usage(program, "invalid option\n");
    }
  }
  if (i != argc - 1) {
    usage(program, "cache spec s-E-b-m required\n");
  }

  srand(seed);
  const char *params_spec = argv[i];

  unsigned n_mem_addr_bits;
  CacheSim *cache = make_cache_sim(params_spec, replacement, &n_mem_addr_bits);
  if (!cache) usage(program, "invalid cache params\n");
  do_cache_sim(cache, is_quiet, n_mem_addr_bits, stdin, stdout);
  free_cache_sim(cache);
  return 0;

}
