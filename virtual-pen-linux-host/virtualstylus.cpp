#include <QGuiApplication>
#include <chrono>
#include <QDebug>
#include "virtualstylus.h"
#include "error.h"
#include "uinput.h"
#include "constants.h"
#include "accessory.h"
#include "pressuretranslator.h"
#include "mainwindow.h"

using namespace std::chrono;

VirtualStylus::VirtualStylus(DisplayScreenTranslator * displayScreenTranslator,
                             PressureTranslator * pressureTranslator){
    this->displayScreenTranslator = displayScreenTranslator;
    this->pressureTranslator = pressureTranslator;
}

void VirtualStylus::initializeStylus(){
    Error * err = new Error();
    const char* deviceName = "pen-emu";
    fd = init_uinput_stylus(deviceName, err);

}


void VirtualStylus::handleAccessoryEventData(AccessoryEventData * accessoryEventData){
    Error * err = new Error();
    uint64_t epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    displayEventDebugInfo(accessoryEventData);
    if(accessoryEventData->action == ACTION_DOWN || accessoryEventData->action == ACTION_MOVE){
        if(accessoryEventData->action == ACTION_DOWN){
            send_uinput_event(fd, ET_KEY, EC_KEY_TOUCH, 1, err);
        }
        if(!isPenActive && accessoryEventData->toolType == PEN_TOOL_TYPE){
            send_uinput_event(fd, ET_KEY, EC_KEY_TOOL_PEN, 1, err);
            send_uinput_event(fd, ET_KEY, EC_KEY_TOOL_RUBBER, 0, err);
            isPenActive = true;
        }
        if(accessoryEventData->toolType == ERASER_TOOL_TYPE){
            send_uinput_event(fd, ET_KEY, EC_KEY_TOOL_PEN, 0, err);
            send_uinput_event(fd, ET_KEY, EC_KEY_TOOL_RUBBER, 1, err);
            isPenActive = false;
        }
        int32_t x = 0;
        int32_t y = 0;
        if(displayScreenTranslator->displayStyle == DisplayStyle::stretched){
            x = displayScreenTranslator->getAbsXStretched(accessoryEventData);
            y = displayScreenTranslator->getAbsYStretched(accessoryEventData);
        }
        else if(displayScreenTranslator->displayStyle == DisplayStyle::fixed){
            x = displayScreenTranslator->getAbsXFixed(accessoryEventData);
            y = displayScreenTranslator->getAbsYFixed(accessoryEventData);

        }

        int pressure = pressureTranslator->getResultingPressure(accessoryEventData);
        send_uinput_event(fd, ET_ABSOLUTE, EC_ABSOLUTE_X, x, err);
        send_uinput_event(fd, ET_ABSOLUTE, EC_ABSOLUTE_Y, y, err);
        send_uinput_event(fd, ET_ABSOLUTE, EC_ABSOLUTE_PRESSURE, pressure, err);
    }
    else{
        send_uinput_event(fd, ET_KEY, EC_KEY_TOOL_PEN, 0, err);
        send_uinput_event(fd, ET_KEY, EC_KEY_TOOL_RUBBER, 0, err);
        send_uinput_event(fd, ET_ABSOLUTE, EC_ABSOLUTE_PRESSURE, 0, err);
        isPenActive = false;
    }

    send_uinput_event(fd, ET_MSC, EC_MSC_TIMESTAMP, epoch, err);
    send_uinput_event(fd, ET_SYNC, EC_SYNC_REPORT, 0, err);
    delete err;
}

void VirtualStylus::displayEventDebugInfo(AccessoryEventData * accessoryEventData){
    if(MainWindow::isDebugMode){
        qDebug() << "              ";
        qDebug() << "Event Action: " << accessoryEventData->action;
        qDebug() << "Event Tool type: " << accessoryEventData->toolType;
        qDebug() << "Event pressure: " << accessoryEventData->pressure;
        qDebug() << "Event x pos: " << accessoryEventData->x;
        qDebug() << "Event y pos: " << accessoryEventData->y;
    }
}



void VirtualStylus::destroyStylus(){
    //ioctl(fd, UI_DEV_DESTROY);
    // ::pclose(fd);
}
