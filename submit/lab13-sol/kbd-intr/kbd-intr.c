#include "kbd-intr.h"

#include <ctype.h>
#include <stdio.h>

#include <unistd.h>
void loop(void *mem_) {
  KbdIntr *mem = mem_;
  do {
    pause();
    //TODO
  } while (1);
}
