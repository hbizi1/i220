#include "ex-connect-data.h"
#include "active-mem.h"

#include "errors.h"

#include <stdio.h>

#include <sys/select.h>
#include <unistd.h>

void
do_connect_data_parent(const Ipc *ipc)
{
  ConnectData *shared = ipc->shared;
  int fd = ipc->fd;
  fd_set read_fds;
  size_t n_fds = (fd > STDIN_FILENO ? fd : STDIN_FILENO) + 1;
  do {
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    FD_SET(fd, &read_fds);
    int status = select(n_fds, &read_fds, NULL, NULL, NULL);
    if (status < 0) { error("select failure:"); break; }
    char buf[2];
    if (FD_ISSET(STDIN_FILENO, &read_fds)) {
      if (fgets(buf, sizeof(buf), stdin) != buf) break;
      shared->in = !shared->in;
    }
    else {
      int n = read(fd, buf, 1);
      if (n < 0) break;
      fputs(shared->out ? "1\n" : "0\n", stdout);
      fflush(stdout);
    }
  } while(1);
}

typedef struct {
  const Ipc *ipc;
  ConnectData *mem;
  ConnectData *shared;
} Ctx;

static MemVal read_fn(void *ctx, const void *addr) {
  int val = ((Ctx *)ctx)->shared->in;
  return val;
}

static void write_fn(void *ctx_, const void *addr, MemVal val) {
  const Ctx *ctx = (Ctx *)ctx_;
  ctx->shared->out = val;
  int n = write(ctx->ipc->fd, "1", 1);
  if (n < 0) return;  //just to shut gcc up
}

void
do_connect_data_child(const Ipc *ipc)
{
  Ctx ctx;
  ConnectData *mem = active_mem_calloc(sizeof(ConnectData), &ctx,
                                       read_fn, write_fn);
  ctx.ipc = ipc; ctx.mem = mem; ctx.shared = ipc->shared;
  loop(mem);
}
