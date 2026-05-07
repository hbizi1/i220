#include "cache-sim.h"

#include "memalloc.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>


/************************** Type Definitions  **************************/

typedef struct {
  bool valid;
  bool dirty;
  MemAddr tag;
  unsigned long access;
} CacheLine;

typedef struct {
  CacheLine *lines;
} CacheSet;

struct CacheSimImpl {
  CacheParams params;
  CacheSet *sets;
  unsigned long clock;
};

/******************** Creation / Destruction Routines ******************/

CacheSim *
new_cache_sim(const CacheParams *params)
{
  CacheSim *cache = malloc_chk(sizeof(struct CacheSimImpl));
  cache->params = *params;
  cache->clock = 0;
  size_t n_sets = 1UL << params->n_set_index_bits;
  cache->sets = malloc_chk(n_sets * sizeof(CacheSet));
  for (size_t i = 0; i < n_sets; i++) {
    cache->sets[i].lines = calloc_chk(params->n_lines_per_set, sizeof(CacheLine));
  }
  return cache;
}

void
free_cache_sim(CacheSim *cache)
{
  if (!cache) return;
  size_t n_sets = 1UL << cache->params.n_set_index_bits;
  for (size_t i = 0; i < n_sets; i++) {
    free(cache->sets[i].lines);
  }
  free(cache->sets);
  free(cache);
}

/************************* Simulation Routine **************************/

CacheResult
cache_sim_result(CacheSim *cache, MemAddr access_addr, bool is_write)
{
  CacheParams p = cache->params;
  cache->clock++;
  unsigned b = p.n_blk_offset_bits;
  unsigned s = p.n_set_index_bits;
  MemAddr block_addr = access_addr >> b;
  MemAddr set_index = block_addr & ((1UL << s) - 1);
  MemAddr tag = block_addr >> s;
  CacheSet *set = &cache->sets[set_index];
  CacheResult result = { .access_addr = access_addr, .is_dirty = false };
  for (unsigned i = 0; i < p.n_lines_per_set; i++) {
    if (set->lines[i].valid && set->lines[i].tag == tag) {
      set->lines[i].access = cache->clock;
      if (is_write) set->lines[i].dirty = true;
      result.status = CACHE_HIT;
      return result;
    }
  }
  for (unsigned i = 0; i < p.n_lines_per_set; i++) {
    if (!set->lines[i].valid) {
      set->lines[i].valid = true;
      set->lines[i].tag = tag;
      set->lines[i].access = cache->clock;
      set->lines[i].dirty = is_write;
      result.status = CACHE_MISS_WITHOUT_REPLACE;
      return result;
    }
  }
  unsigned victim_idx = 0;
  if (p.replacement == RANDOM_R) {
    victim_idx = rand() % p.n_lines_per_set;
  } else {
    unsigned long target_time = set->lines[0].access;
    for (unsigned i = 1; i < p.n_lines_per_set; i++) {
      bool is_lru_better = (p.replacement == LRU_R && set->lines[i].access < target_time);
      bool is_mru_better = (p.replacement == MRU_R && set->lines[i].access > target_time);
      if (is_lru_better || is_mru_better) {
        target_time = set->lines[i].access;
        victim_idx = i;
      }
    }
  }
  CacheLine *victim = &set->lines[victim_idx];
  result.replace_addr = (victim->tag << (s + b)) | (set_index << b);
  result.is_dirty = victim->dirty;
  result.status = CACHE_MISS_WITH_REPLACE;
  victim->tag = tag;
  victim->access = cache->clock;
  victim->dirty = is_write;
  return result;
}
