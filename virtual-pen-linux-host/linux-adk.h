/*
 * Linux ADK - linux-adk.h
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

#ifndef _LINUX_ADK_H_
#define _LINUX_ADK_H_

#include <stdint.h>
#include <unistd.h>
#include "virtualstylus.h"


/* Android Open Accessory protocol defines */
#define AOA_GET_PROTOCOL		51
#define AOA_SEND_IDENT			52
#define AOA_START_ACCESSORY		53
#define AOA_REGISTER_HID		54
#define AOA_UNREGISTER_HID		55
#define AOA_SET_HID_REPORT_DESC		56
#define AOA_SEND_HID_EVENT		57
#define AOA_AUDIO_SUPPORT		58

/* String IDs */
#define AOA_STRING_MAN_ID		0
#define AOA_STRING_MOD_ID		1
#define AOA_STRING_DSC_ID		2
#define AOA_STRING_VER_ID		3
#define AOA_STRING_URL_ID		4
#define AOA_STRING_SER_ID		5

/* Product IDs / Vendor IDs */
#define AOA_ACCESSORY_VID		0x18D1	/* Google */
#define AOA_ACCESSORY_PID		0x2D00	/* accessory */
#define AOA_ACCESSORY_ADB_PID		0x2D01	/* accessory + adb */
#define AOA_AUDIO_PID			0x2D02	/* audio */
#define AOA_AUDIO_ADB_PID		0x2D03	/* audio + adb */
#define AOA_ACCESSORY_AUDIO_PID		0x2D04	/* accessory + audio */
#define AOA_ACCESSORY_AUDIO_ADB_PID	0x2D05	/* accessory + audio + adb */

/* Endpoint Addresses TODO get from interface descriptor */
#define AOA_ACCESSORY_EP_IN		0x81
#define AOA_ACCESSORY_EP_OUT		0x02
#define AOA_ACCESSORY_INTERFACE		0x00

/* App defines */
#define PACKAGE_VERSION		"0.4"
#define PACKAGE_BUGREPORT	"bisson.gary@gmail.com"

/* Variable to stop accessory */
extern volatile int stop_acc;

/* Structures */
typedef struct _accessory_t {
	struct libusb_device_handle *handle;
	struct libusb_transfer *transfer;
	uint32_t aoa_version;
	uint16_t vid;
	uint16_t pid;
	char *device;
	char *manufacturer;
	char *model;
	char *description;
	char *version;
	char *url;
	char *serial;
} accessory_t;

int capture(string selectedDevice, VirtualStylus * mainWindow);

#endif /* _LINUX_ADK_H_ */
