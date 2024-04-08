#include "displayscreentranslator.h"
#include "constants.h"

#include <qguiapplication.h>

DisplayScreenTranslator::DisplayScreenTranslator() {
    screen = QGuiApplication::primaryScreen();
}

int32_t DisplayScreenTranslator::getAbsXStretched(AccessoryEventData * accessoryEventData){
    return getStretchedSize(accessoryEventData->x, size_x);
}

int32_t DisplayScreenTranslator::getAbsYStretched(AccessoryEventData * accessoryEventData){
    return getStretchedSize(accessoryEventData->y, size_y);
}


int32_t DisplayScreenTranslator::getStretchedSize(int posOnDevice, int accessorySize){
    float accessorySizeFloat = accessorySize;
    return static_cast<int>((posOnDevice / accessorySizeFloat) * ABS_MAX_VAL);
}

int32_t DisplayScreenTranslator::getAbsXFixed(AccessoryEventData * accessoryEventData){
    return getFixedSize(accessoryEventData->x, size_x, getScreenX());
}

int32_t DisplayScreenTranslator::getAbsYFixed(AccessoryEventData * accessoryEventData){
    return getFixedSize(accessoryEventData->y, size_y, getScreenY());
}

int32_t DisplayScreenTranslator::getFixedSize(int posOnDevice, int accessorySize, int mainScreenSize){
    float diff = mainScreenSize - accessorySize;
    if(diff >= 0){
        float pos = ((diff / 2) + posOnDevice) / mainScreenSize;
        return static_cast<int>(pos * ABS_MAX_VAL);
    }
    else{
        return getStretchedSize(posOnDevice, accessorySize);
    }
}


int DisplayScreenTranslator::getScreenX(){
    return screen->geometry().width();
}

int DisplayScreenTranslator::getScreenY(){
    return screen->geometry().height();
}
