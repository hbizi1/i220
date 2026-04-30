#define _POSIX_C_SOURCE 1
#define _GNU_SOURCE

#include "ex-busy-data.h"
#include "ex-connect-data.h"
#include "ex-kbd-intr.h"
#include "ex-scan-keys.h"

#include "errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#if 0
static void
cleanup_child(pid_t child_pid)
{
  if (kill(child_pid, SIGTERM) != 0) {
     panic("cannot kill child %lld:", (long long)child_pid);
  }
  if (wait(NULL) < 0) {
    panic("could not wait for child %lld:", (long long)child_pid);
  }
}
#endif

typedef void ParentFn(const Ipc *ipc);
typedef  void ChildFn(const Ipc *ipc);

//void halt() { exit(0); }

static void
setup_processes(ParentFn *parent_fn, ChildFn *child_fn)
{
  int fds[2];
  if (pipe2(fds, O_NONBLOCK) < 0) {  //child to parent to signal data avail
    panic("cannot create child-to-parent pipe:");
  }
  enum { SHARED_AREA_SIZE = 1024 };
  void *shared =
    mmap(NULL, SHARED_AREA_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED,
         -1, 0);
  if (shared == (void *)-1) panic("cannot create shared data area:");
  pid_t pid = fork();
  if (pid < 0) {
    panic("cannot fork child:");
  }
  else if (pid > 0) { //parent
    close(fds[1]);
    Ipc ipc = { .shared=shared, .fd=fds[0], .other_pid=pid, };
    parent_fn(&ipc);
    //cleanup_child(pid);
  }
  else { //child
    fclose(stdin);
    if (freopen("/dev/tty", "w", stdout) == NULL) {
      panic("cannot reopen stdout in child:");
    }
    close(fds[0]);
    Ipc ipc = { .shared = shared, .fd = fds[1], .other_pid=getppid(), };
    child_fn(&ipc);
  }

}

typedef struct {
  const char *name;
  ParentFn *parent_fn;
  ChildFn *child_fn;
} ExerciseInfo;

static ExerciseInfo infos[] = {
  { .name="busy-data",
    .parent_fn=do_busy_data_parent, .child_fn=do_busy_data_child
  },
  { .name="connect-data",
    .parent_fn=do_connect_data_parent, .child_fn=do_connect_data_child
  },
  { .name="kbd-intr",
    .parent_fn=do_kbd_intr_parent, .child_fn=do_kbd_intr_child
  },
  { .name="scan-keys",
    .parent_fn=do_scan_keys_parent, .child_fn=do_scan_keys_child
  },
};


int
main(int argc, const char *argv[])
{
  const char *exec = argv[0];
  const char *last_slash = strrchr(exec, '/');
  if (last_slash == NULL) {
    panic("executable path %s does not contain a '/'", exec);
  }
  const char *name = last_slash + 1;
  const ExerciseInfo *p = NULL;
  for (int i = 0; i < sizeof(infos)/sizeof(infos[0]); i++) {
    if (strcmp(infos[i].name, name) == 0) {
      p = &infos[i]; break;
    }
  }
  if (p == NULL) panic("no exercise info for %s", name);
  setup_processes(p->parent_fn, p->child_fn);
  return 0;
}
