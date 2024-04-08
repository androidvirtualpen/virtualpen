#ifndef PRESSURETRANSLATOR_H
#define PRESSURETRANSLATOR_H

#include "accessory.h"
class PressureTranslator
{
public:
    PressureTranslator();
    int sensitivity = 50;
    int minPressure = 0;
    int getResultingPressure(AccessoryEventData * accessoryEventData);
private:
    float getPressureSensitivityPercentage();
};

#endif // PRESSURETRANSLATOR_H
