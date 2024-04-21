#ifndef DISPLAYSCREENTRANSLATOR_H
#define DISPLAYSCREENTRANSLATOR_H
#include "accessory.h"
#include <QScreen>

enum DisplayStyle{
    stretched,
    fixed
};

class DisplayScreenTranslator{
public:
    int size_x;
    int size_y;
    DisplayStyle displayStyle;
    int32_t getAbsXStretched(AccessoryEventData * pos);
    int32_t getAbsYStretched(AccessoryEventData * pos);
    int32_t getAbsXFixed(AccessoryEventData * pos);
    int32_t getAbsYFixed(AccessoryEventData * pos);
    DisplayScreenTranslator();
private:
    QScreen *screen;
    int getScreenX();
    int getScreenY();
    int32_t getStretchedSize(int posOnDevice, int accessorySize);
    int32_t getFixedSize(int posOnDevice, int accessorySize, int mainScreenSize);
};


#endif // DISPLAYSCREENTRANSLATOR_H
