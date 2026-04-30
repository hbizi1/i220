#define _POSIX_C_SOURCE 200809L

#include "ex-scan-keys.h"
#include "active-mem.h"

#include "errors.h"

#include <stdio.h>

#include <signal.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

void
do_scan_keys_parent(const Ipc *ipc)
{
  ScanKeys *shared = ipc->shared;
  int fd = ipc->fd;
  fd_set read_fds;
  size_t n_fds = (fd > STDIN_FILENO ? fd : STDIN_FILENO) + 1;
  do {
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    FD_SET(fd, &read_fds);
    int status = select(n_fds, &read_fds, NULL, NULL, NULL);
    if (status < 0) { error("select failure:"); break; }
    if (FD_ISSET(STDIN_FILENO, &read_fds)) {
      if (scanf("%d|%d", &shared->row, &shared->col) != 2) break;
      kill(ipc->other_pid, SIGUSR1);
    }
    else {
      char buf[2];
      int n = read(fd, buf, 1);
      if (n < 0) break;
      fprintf(stdout, "%c\n", shared->out);
      fflush(stdout);
    }
  } while(1);
}

typedef struct {
  const Ipc *ipc;
  ScanKeys *mem;
  ScanKeys *shared;
} Ctx;

static MemVal read_fn(void *ctx_, const void *addr) {
  const Ctx *ctx = ctx_;
  const size_t offset = (char *)addr - (char *)ctx->mem;
  //would use offsetof with switch if more possibilities
  int val = (offset == 0) ? ctx->shared->row : ctx->shared->col;
  return val;
}

static void write_fn(void *ctx_, const void *addr, MemVal val) {
  const Ctx *ctx = (Ctx *)ctx_;
  ctx->shared->out = val;
  int n = write(ctx->ipc->fd, "1", 1);
  if (n < 0) return;  //just to shut gcc up
}

static void sigusr1_handler(int signo, siginfo_t *siginfo, void *ctx) {
  return;
}

void
do_scan_keys_child(const Ipc *ipc)
{
  Ctx ctx;
  ScanKeys *mem = active_mem_calloc(sizeof(ScanKeys), &ctx,
                                    read_fn, write_fn);
  ctx.ipc = ipc; ctx.mem = mem; ctx.shared = ipc->shared;
  struct sigaction sa = { .sa_sigaction = sigusr1_handler };
  if (sigaction(SIGUSR1, &sa, NULL) < 0) {
    panic("cannot set up handler for SIGUSR1:");
  }
  loop(mem);
}
