#include "pressuretranslator.h"
#include "constants.h"

PressureTranslator::PressureTranslator() {}

int PressureTranslator::getResultingPressure(AccessoryEventData * accessoryEventData){
    float resultingPressurePercentage = 0;
    float minPressurePrecentage = (float) minPressure / 100.0;
    if(accessoryEventData->pressure > minPressurePrecentage){
        resultingPressurePercentage = (accessoryEventData->pressure - minPressurePrecentage) * getPressureSensitivityPercentage();
    }
    return static_cast<int>(resultingPressurePercentage * ABS_MAX_VAL);
}

float PressureTranslator::getPressureSensitivityPercentage(){
    return sensitivity / 50.0;
}


