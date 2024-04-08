#ifndef CONSTANTS_H
#define CONSTANTS_H
const int ABS_MAX_VAL= 65535;

const int ET_SYNC = 0x00;
const int ET_KEY = 0x01;
const int ET_RELATIVE = 0x02;
const int ET_ABSOLUTE = 0x03;
const int ET_MSC = 0x04;

// Event Codes
const int EC_SYNC_REPORT = 0;

const int EC_KEY_TOOL_PEN = 0x140;
const int EC_KEY_TOOL_RUBBER = 0x141;
const int EC_KEY_TOUCH = 0x14a;

const int EC_ABSOLUTE_X = 0x00;
const int EC_ABSOLUTE_Y = 0x01;
const int EC_ABSOLUTE_PRESSURE = 0x18;
const int EC_ABSOLUTE_TILT_X = 0x1a;
const int EC_ABSOLUTE_TILT_Y = 0x1b;

const int EC_MSC_TIMESTAMP = 0x05;

const int PEN_TOOL_TYPE = 2;
const int ERASER_TOOL_TYPE = 4;

#endif // CONSTANTS_H
