#ifndef VIRTUALSTYLUS_H
#define VIRTUALSTYLUS_H
#include "accessory.h"
#include "displayscreentranslator.h"
#include "pressuretranslator.h"
#include <QScreen>

class VirtualStylus
{
public:
    VirtualStylus(DisplayScreenTranslator * accessoryScreen, PressureTranslator *pressureTranslator);
    void handleAccessoryEventData(AccessoryEventData * accessoryEventData);
    void initializeStylus();
    void destroyStylus();

private:
    int fd;
    bool isPenActive;
    DisplayScreenTranslator * displayScreenTranslator;
    PressureTranslator * pressureTranslator;
    void displayEventDebugInfo(AccessoryEventData * accessoryEventData);
};
#endif // VIRTUALSTYLUS_H
