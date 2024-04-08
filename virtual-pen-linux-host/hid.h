/*
 * Linux ADK - hid.h
 *
 * Copyright (C) 2014 - Gary Bisson <bisson.gary@gmail.com>
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

#ifndef _HID_H_
#define _HID_H_

#include <pthread.h>
#include "linux-adk.h"

/* Structures */
typedef struct {
	struct libusb_device_handle *handle;
	unsigned char descriptor[256];
	int descriptor_size;
	int endpoint_in;
    size_t packet_size;
	pthread_t rx_thread;
} hid_device;

/* Functions */
extern int send_hid_descriptor(accessory_t *acc, hid_device *hid);
extern int register_hid_callback(accessory_t* acc, hid_device *hid);
extern unsigned char search_hid(hid_device *hid);

#endif /* _HID_H_ */
