#include "ex-busy-data.h"
#include "active-mem.h"

#include <stdio.h>

void
do_busy_data_parent(const Ipc *ipc)
{
  BusyData *shared = ipc->shared;
  do {
    char buf[2];
    if (fgets(buf, sizeof(buf), stdin) != buf) break;
    shared->data = !shared->data;
  } while(1);
}

typedef struct {
  BusyData *mem;
  BusyData *shared;
} Ctx;

static MemVal read_fn(void *ctx, const void *addr) {
  return ((Ctx *)ctx)->shared->data;
}

void
do_busy_data_child(const Ipc *ipc)
{
  Ctx ctx;
  BusyData *mem = active_mem_calloc(sizeof(BusyData), &ctx, read_fn, NULL);
  ctx.mem = mem; ctx.shared = ipc->shared;
  loop(mem);
}
