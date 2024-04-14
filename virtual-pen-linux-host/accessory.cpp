/*
 * Linux ADK - accessory.c
 *
 * Copyright (C) 2013 - Gary Bisson <bisson.gary@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <libusb-1.0/libusb.h>
#include "virtualstylus.h"
#include "accessory.h"
#include "linux-adk.h"
#include "mainwindow.h"


using namespace std;

void extractAccessoryEventData(AccessoryEventData * accessoryEventData,
                                              unsigned char* dataBuffer, int size)
{

    array<string, 5> strs = readUntilDelimiter(dataBuffer, size);
    accessoryEventData->toolType = std::stoi(strs[0]);
    accessoryEventData->action = std::stoi(strs[1]);
    accessoryEventData->x = std::stoi(strs[2]);
    accessoryEventData->y = std::stoi(strs[3]);
    if(strs[4] != ""){
        accessoryEventData->pressure = std::stod(strs[4]);
    }
}

array<string, 5> readUntilDelimiter(unsigned char* dataBuffer, int size){
    array<string, 5> strs;
    int strPos = 0;
    string currentString = "";
    for (int i = 0; i < size; i++) {
        char c = (char) dataBuffer[i];
        if(c == ','){
            strs[strPos] = currentString;
            currentString = "";
            strPos += 1;
        }
        else{
            currentString += c;
        }
    }
    return strs;
}

void accessory_main(accessory_t * acc, VirtualStylus* virtualStylus)
{
    int ret = 0;
    /* If we have an accessory interface */
    if ((acc->pid != AOA_AUDIO_ADB_PID) && (acc->pid != AOA_AUDIO_PID)) {
        unsigned char acc_buf[512];
		int transferred, i;
		int errors = 20;

		/* Claiming first (accessory )interface from the opened device */
        ret = libusb_claim_interface(acc->handle, AOA_ACCESSORY_INTERFACE);
		if (ret != 0) {
			printf("Error %d claiming interface...\n", ret);
			return;
		}

		/* Snooping loop; Display every data received from device */
        AccessoryEventData * accessoryEventData = new AccessoryEventData();
		while (!stop_acc) {
			ret =
			    libusb_bulk_transfer(acc->handle,
						 AOA_ACCESSORY_EP_IN, acc_buf,
						 sizeof(acc_buf), &transferred,
						 200);

			if (ret < 0) {
				if (ret == LIBUSB_ERROR_TIMEOUT)
					continue;
				printf("bulk transfer error %d\n", ret);
				if (--errors == 0)
					break;
				else
					sleep(1);
			}
            extractAccessoryEventData(accessoryEventData, acc_buf, transferred);
            if(MainWindow::isDebugMode){
                printf("Received %d bytes\n", transferred);
                for (i = 0; i < transferred;) {
                    printf("%#2.2x ", acc_buf[i++]);
                    if (!(i % 8))
                        printf("\n");
                }
                printf("\n");
            }
            virtualStylus->handleAccessoryEventData(accessoryEventData);
		}
        delete accessoryEventData;
	}
}
