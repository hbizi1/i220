#ifndef EX_KBD_INTR_H_
#define EX_KBD_INTR_H_

#include "exercises.h"
#include "kbd-intr.h"

void do_kbd_intr_parent(const Ipc *ipc);
void do_kbd_intr_child(const Ipc *ipc);

#endif // #ifndef EX_KBD_INTR_H_
