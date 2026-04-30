#ifndef BUSY_DATA_H_
#define BUSY_DATA_H_

typedef struct {
  volatile _Atomic int data;  //non-zero echoed to console as 1, else 0
} BusyData;


#endif //#ifndef BUSY_DATA_H_
