#ifndef KBD_INTR_H_
#define KBD_INTR_H_

typedef struct {
  volatile _Atomic int in;   //character from in
  volatile _Atomic int out;  //is copied to out with case toggled.
} KbdIntr;


#endif //#ifndef KBD_INTR_H_
