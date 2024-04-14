/*
 * Linux ADK - linux-adk.c
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
#include <signal.h>
#include <linux/uinput.h>
#include </usr/include/libusb-1.0/libusb.h>
#include "linux-adk.h"
#include "virtualstylus.h"

extern void accessory_main(accessory_t * acc, VirtualStylus * virtualStylys);

volatile int stop_acc = 0;
int verbose = 0;

std::string device = "18d1:4ee2";
std::string manufacturer = "mbeliveau";
std::string model = "VirtualStylusHost";
std::string description = "Linux Virtual Stylus Host";
std::string version = "1.0";
std::string url = "https://github.com/Mathieu-Beliveau/android-pen-emulator";
std::string serial = "0000000012345678";

static const accessory_t acc_default = {
    .device = device.data(),
    .manufacturer = manufacturer.data(),
    .model = model.data(),
    .description = description.data(),
    .version = version.data(),
    .url = url.data(),
    .serial = serial.data(),
};

static int is_accessory_present(accessory_t * acc);
static int init_accessory(accessory_t * acc, int aoa_max_version);
static void fini_accessory(accessory_t * acc);


static void show_version(char *name)
{
	printf("%s v%s\nreport bugs to %s\n", name, PACKAGE_VERSION,
	       PACKAGE_BUGREPORT);
	return;
}

static void signal_handler(int signo)
{
	printf("SIGINT: Closing accessory\n");
	stop_acc = 1;
}

int capture(string selectedDevice, VirtualStylus* virtualStylus)
{
	int arg_count = 1;
	int no_app = 0;
	int aoa_max_version = -1;
	accessory_t acc = { NULL, NULL, 0, 0, 0, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL
	};

	if (signal(SIGINT, signal_handler) == SIG_ERR)
		printf("Cannot setup a signal handler...\n");

	/* Disable buffering on stdout */
    setbuf(stdout, NULL);
    acc.device = selectedDevice.data();
    acc.serial = acc_default.serial;
    acc.manufacturer = acc_default.manufacturer;
    acc.model = acc_default.model;
    acc.version = acc_default.version;
#ifdef WIN32
	/* AOA 2.0 not supported on Windows (pthread/hid/audio deps) */
	aoa_max_version = 1;
#endif
	if (init_accessory(&acc, aoa_max_version) != 0)
		goto end;

    accessory_main(&acc, virtualStylus);

end:
	fini_accessory(&acc);
	return 0;
}

static int init_accessory(accessory_t * acc, int aoa_max_version)
{
	int ret;
	uint16_t pid, vid;
	char *tmp;
	uint8_t buffer[2];
	int tries = 10;

	/* Initializing libusb */
	ret = libusb_init(NULL);
	if (ret != 0) {
		printf("libusb init failed: %d\n", ret);
		return ret;
	}
	if (verbose)
		libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);

	/* Check if device is not already in accessory mode */
	if (is_accessory_present(acc))
		return 0;

	/* Getting product and vendor IDs */
	vid = (uint16_t) strtol(acc->device, &tmp, 16);
	pid = (uint16_t) strtol(tmp + 1, &tmp, 16);
	printf("Looking for device %4.4x:%4.4x\n", vid, pid);

	/* Trying to open it */
	acc->handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
	if (acc->handle == NULL) {
		printf("Unable to open device...\n");
		return -1;
	}

	/* Now asking if device supports Android Open Accessory protocol */
	ret = libusb_control_transfer(acc->handle,
				      LIBUSB_ENDPOINT_IN |
				      LIBUSB_REQUEST_TYPE_VENDOR,
				      AOA_GET_PROTOCOL, 0, 0, buffer,
				      sizeof(buffer), 0);
	if (ret < 0) {
		printf("Error getting protocol...\n");
		return ret;
	} else {
		acc->aoa_version = ((buffer[1] << 8) | buffer[0]);
		printf("Device supports AOA %d.0!\n", acc->aoa_version);
	}
	if ((aoa_max_version > 0) && ((int)acc->aoa_version > aoa_max_version)) {
		acc->aoa_version = aoa_max_version;
		printf("Limiting AOA to version %d.0!\n", acc->aoa_version);
	}

	/* Some Android devices require a waiting period between transfer calls */
	usleep(10000);

	/* In case of a no_app accessory, the version must be >= 2 */
	if ((acc->aoa_version < 2) && !acc->manufacturer) {
		printf("Connecting without an Android App only for AOA 2.0\n");
		return -1;
	}

	printf("Sending identification to the device\n");


    if (acc->manufacturer) {
        printf(" sending manufacturer: %s\n", acc->manufacturer);
        ret = libusb_control_transfer(acc->handle,
                                      LIBUSB_ENDPOINT_OUT
                                          | LIBUSB_REQUEST_TYPE_VENDOR,
                                      AOA_SEND_IDENT, 0,
                                      AOA_STRING_MAN_ID,
                                      (uint8_t *) acc->manufacturer,
                                      strlen(acc->manufacturer) + 1, 0);
        if (ret < 0)
            goto error;
    }

    if (acc->model) {
        printf(" sending model: %s\n", acc->model);
        ret = libusb_control_transfer(acc->handle,
                                      LIBUSB_ENDPOINT_OUT
                                          | LIBUSB_REQUEST_TYPE_VENDOR,
                                      AOA_SEND_IDENT, 0,
                                      AOA_STRING_MOD_ID,
                                      (uint8_t *) acc->model,
                                      strlen(acc->model) + 1, 0);
        if (ret < 0)
            goto error;
    }


    printf(" sending version: %s\n", acc->version);
    ret = libusb_control_transfer(acc->handle,
                                  LIBUSB_ENDPOINT_OUT |
                                      LIBUSB_REQUEST_TYPE_VENDOR,
                                  AOA_SEND_IDENT, 0, AOA_STRING_VER_ID,
                                  (uint8_t *) acc->version,
                                  strlen(acc->version) + 1, 0);
    if (ret < 0)
        goto error;


	printf("Turning the device in Accessory mode\n");
	ret = libusb_control_transfer(acc->handle,
				      LIBUSB_ENDPOINT_OUT |
				      LIBUSB_REQUEST_TYPE_VENDOR,
				      AOA_START_ACCESSORY, 0, 0, NULL, 0, 0);
	if (ret < 0)
		goto error;

	/* Let some time for the new enumeration to happen */
	usleep(10000);

	/* Connect to the Accessory */
	while (tries--) {
		if (is_accessory_present(acc))
			break;
		else if (!tries)
			goto error;
		else
			sleep(1);
	}

	return 0;

error:
	printf("Accessory init failed: %d\n", ret);
	return -1;
}

static int is_accessory_present(accessory_t * acc)
{
	struct libusb_device_handle *handle;
	uint16_t vid = AOA_ACCESSORY_VID;
	uint16_t pid = 0;

	/* Trying to open all the AOA IDs possible */
	pid = AOA_ACCESSORY_PID;
	handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
	if (handle != NULL)
		goto claim;

	pid = AOA_ACCESSORY_ADB_PID;
	handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
	if (handle != NULL)
		goto claim;

	return 0;

claim:
	printf("Found accessory %4.4x:%4.4x\n", vid, pid);
	acc->handle = handle;
	acc->vid = vid;
	acc->pid = pid;
	return 1;
}

static void fini_accessory(accessory_t * acc)
{
	printf("Closing USB device\n");

	if (acc->handle != NULL) {
		libusb_release_interface(acc->handle, 0);
		libusb_close(acc->handle);
	}

	libusb_exit(NULL);

	return;
}

