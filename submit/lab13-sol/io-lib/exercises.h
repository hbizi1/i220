#ifndef EXERCISES_H_
#define EXERCISES_H_

#include <sys/types.h>

typedef struct {
  pid_t other_pid;
  void *shared;
  int fd;
} Ipc;

extern void loop(void *mem);

#endif //#ifndef EXERCISES_H_
