#ifndef ACCESSORY_H
#define ACCESSORY_H
#include <string>
#include <array>
using namespace std;

typedef struct _accessoryEventData{
    int toolType;
    int action;
    float pressure;
    int x;
    int y;
} AccessoryEventData;
void printRawInputData(array<string, 5> * strs);
array<string, 5> readUntilDelimiter(unsigned char* a, int size);

#endif // ACCESSORY_H
