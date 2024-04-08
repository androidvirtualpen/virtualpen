#ifndef UINPUT_H
#define UINPUT_H
#include "error.h"
const int ACTION_DOWN = 0;
const int ACTION_MOVE = 2;
const int ACTION_UP = 1;
const int ACTION_CANCEL = 3;
const int ACTION_OUTSIDE = 4;
extern "C" int init_uinput_stylus(const char* name, Error* err);
extern "C" void send_uinput_event(int device, int type, int code, int value, Error* err);
#endif // UINPUT_H
