#ifndef CONNECT_DATA_H_
#define CONNECT_DATA_H_

typedef struct {
  volatile _Atomic int in;   //whatever is read from in
  volatile _Atomic int out;  //is copied to out
} ConnectData;


#endif //#ifndef CONNECT_DATA_H_
