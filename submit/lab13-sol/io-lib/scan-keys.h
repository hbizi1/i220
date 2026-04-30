#ifndef SCAN_KEYS_H_
#define SCAN_KEYS_H_

typedef struct {
  volatile _Atomic unsigned row;   //single-hot row index
  volatile _Atomic unsigned col;   //single-hot col index
  volatile _Atomic int out;        //decoded char
} ScanKeys;


#endif //#ifndef SCAN_KEYS_H_
