/*
 * Linux ADK - hid.c
 *
 * Copyright (C) 2014 - Gary Bisson <bisson.gary@gmail.com>
 *
 * Based on usbAccReadWrite.c by Jeremy Rosen
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
#ifndef WIN32
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <poll.h>
#include <libusb-1.0/libusb.h>

#include "linux-adk.h"
#include "hid.h"

static void *receive_loop(void *arg)
{
	while (!stop_acc) {
		int ret = 0;
		int i;
		int nfds = 1;
		struct timeval tv, zero_tv;
		const struct libusb_pollfd **poll_list;
		fd_set rfds, wfds, efds;
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		zero_tv.tv_sec = 0;
		zero_tv.tv_usec = 0;

		ret = libusb_get_next_timeout(NULL, &tv);
		if (ret < 0) {
			if (ret)
				printf("USB error : %s\n",
				       libusb_error_name(ret));
			break;
		}

		if (ret == 1 && tv.tv_sec == 0 && tv.tv_usec == 0) {
			ret = libusb_handle_events_timeout_completed(NULL,
								     &zero_tv,
								     NULL);
			if (ret) {
				if (ret)
					printf("USB error : %s\n",
					       libusb_error_name(ret));
				break;
			}
			continue;
		}

		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		FD_SET(0, &rfds);
		poll_list = libusb_get_pollfds(NULL);
		if (poll_list) {
			for (i = 0; poll_list[i] != NULL; i++) {
				const struct libusb_pollfd *cur_poll;

				cur_poll = poll_list[i];
				nfds = cur_poll->fd >= nfds
					? cur_poll->fd + 1 : nfds;
				if (cur_poll->events & POLLIN)
					FD_SET(cur_poll->fd, &rfds);
				if (cur_poll->events & POLLOUT)
					FD_SET(cur_poll->fd, &wfds);
			}
			free(poll_list);
		} else
			break;

		if (tv.tv_sec == 0 && tv.tv_usec == 0)
			ret = select(nfds, &rfds, &wfds, &efds, NULL);
		else
			ret = select(nfds, &rfds, &wfds, &efds, &tv);

		if (libusb_handle_events_timeout_completed
		    (NULL, &zero_tv, NULL))
			break;
	}

	return NULL;
}

static int open_device(struct libusb_device_handle **handle,
		      libusb_device * device, int interface)
{
	int kernel_claimed = 0, ret;

	libusb_ref_device(device);
	ret = libusb_open(device, handle);
	if (ret) {
		printf("Unable to open usb device [%#08x]\n", ret);
		*handle = NULL;
		return -1;
	}

	ret = libusb_kernel_driver_active(*handle, interface);
	if (ret == 1) {
		if (libusb_detach_kernel_driver(*handle, interface)) {
			printf("Unable to grab usb device\n");
			libusb_close(*handle);
			*handle = NULL;
			return -1;
		}
		kernel_claimed = 1;
	}

	ret = libusb_claim_interface(*handle, interface);
	if (ret) {
		printf("Failed to claim interface %d.\n", interface);
		if (kernel_claimed) {
			libusb_attach_kernel_driver(*handle, interface);
			kernel_claimed = 0;
		}
		libusb_close(*handle);
		*handle = NULL;
		return -1;
	}

	return 0;
}

static void callback_hid(struct libusb_transfer *transfer)
{
    accessory_t *acc = (accessory_t*) transfer->user_data;
	int rc = 0;

	if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {
		struct libusb_transfer *android_transfer;
		unsigned char *keybuf;
		int rc;

		android_transfer = libusb_alloc_transfer(0);
        keybuf = (unsigned char*) malloc(transfer->actual_length + LIBUSB_CONTROL_SETUP_SIZE);
		memcpy(keybuf + LIBUSB_CONTROL_SETUP_SIZE, transfer->buffer,
		       transfer->actual_length);

		libusb_fill_control_setup(keybuf,
					  LIBUSB_ENDPOINT_OUT |
					  LIBUSB_REQUEST_TYPE_VENDOR,
					  AOA_SEND_HID_EVENT, 1, 0,
					  transfer->actual_length);

		libusb_fill_control_transfer(android_transfer, acc->handle,
					     keybuf, NULL, NULL, 0);

		android_transfer->flags =
		    LIBUSB_TRANSFER_FREE_BUFFER | LIBUSB_TRANSFER_FREE_TRANSFER;

		rc = libusb_submit_transfer(android_transfer);
		if (rc)
			printf("USB error : %s\n", libusb_error_name(rc));

		rc = libusb_submit_transfer(transfer);
		if (rc)
			printf("USB error : %s\n", libusb_error_name(rc));

	} else if (transfer->status == LIBUSB_TRANSFER_TIMED_OUT) {
		rc = libusb_submit_transfer(transfer);
		if (rc)
			printf("USB error : %s\n", libusb_error_name(rc));
	}
}

unsigned char search_hid(hid_device * hid)
{
	libusb_device **list;
	struct libusb_device_descriptor desc;
	ssize_t cnt;
	ssize_t i = 0;
	int j;
	libusb_device *device;

	hid->handle = NULL;

	/* List every USB device attached */
	cnt = libusb_get_device_list(NULL, &list);
	if (cnt < 0)
		goto error0;

	for (i = 0; i < cnt; i++) {
		device = list[i];
		int r;

		r = libusb_get_device_descriptor(device, &desc);
		if (r < 0)
			continue;

		if (desc.bDeviceClass == LIBUSB_CLASS_HID) {
			goto found;
			break;
		} else if (desc.bDeviceClass == LIBUSB_CLASS_PER_INTERFACE) {
			struct libusb_config_descriptor *current_config;
			int r;

			r = libusb_get_active_config_descriptor(device,
								&current_config);
			if (r < 0)
				continue;

			for (j = 0; j < current_config->bNumInterfaces; j++) {
				int k;
				for (k = 0;
				     k <
				     current_config->
				     interface[j].num_altsetting; k++) {
					if (current_config->
					    interface[j].altsetting[k].
					    bInterfaceClass ==
					    LIBUSB_CLASS_HID) {
						hid->endpoint_in =
						    current_config->interface
						    [j].
						    altsetting[k].endpoint
						    [0].bEndpointAddress;
						hid->packet_size =
						    current_config->interface
						    [j].
						    altsetting[k].endpoint[0].
						    wMaxPacketSize;
						goto found;
					}
				}
			}
			libusb_free_config_descriptor(current_config);
		}
	}
found:
	if (i == cnt)
		goto error0;

	if (open_device(&hid->handle, device, j) < 0)
		goto error0;

	hid->descriptor_size = libusb_control_transfer(hid->handle,
						       LIBUSB_ENDPOINT_IN |
						       LIBUSB_RECIPIENT_INTERFACE,
						       LIBUSB_REQUEST_GET_DESCRIPTOR,
						       LIBUSB_DT_REPORT << 8, 0,
						       hid->descriptor, 256, 0);
	if (hid->descriptor_size < 0)
		goto error1;

	libusb_free_device_list(list, 1);
	printf("=> found HID device vid 0x%x pid 0x%x\n", desc.idVendor,
	       desc.idProduct);
	return 0;
error1:
	libusb_close(hid->handle);
error0:
	libusb_free_device_list(list, 1);
	return -1;
}

int register_hid_callback(accessory_t * acc, hid_device * hid)
{
	struct libusb_transfer *hid_transfer;
	unsigned char *keybuf;
	int rc;

    keybuf = (unsigned char*) malloc(hid->packet_size);

	hid_transfer = libusb_alloc_transfer(0);
	if (hid_transfer == NULL) {
		libusb_close(hid->handle);
		return -1;
	}

	libusb_fill_interrupt_transfer(hid_transfer, hid->handle,
				       hid->endpoint_in,
				       keybuf, hid->packet_size, callback_hid,
				       acc, 0);

	rc = libusb_submit_transfer(hid_transfer);
	if (rc != 0) {
		if (rc)
			printf("USB error : %s\n", libusb_error_name(rc));
		libusb_close(hid->handle);
		return -1;
	}

	return 0;
}

int send_hid_descriptor(accessory_t * acc, hid_device * hid)
{
	int ret;

	ret = libusb_control_transfer(acc->handle, LIBUSB_ENDPOINT_OUT |
				      LIBUSB_REQUEST_TYPE_VENDOR,
				      AOA_REGISTER_HID, 1, hid->descriptor_size,
				      NULL, 0, 0);
	if (ret < 0) {
		printf("couldn't register HID device on the android device : %s\n",
		       libusb_error_name(ret));
		libusb_close(hid->handle);
		return -1;
	}

	ret = libusb_control_transfer(acc->handle, LIBUSB_ENDPOINT_OUT |
				      LIBUSB_REQUEST_TYPE_VENDOR,
				      AOA_SET_HID_REPORT_DESC, 1, 0,
				      hid->descriptor, hid->descriptor_size, 0);
	if (ret < 0) {
		printf("couldn't send HID descriptor to the android device\n");
		libusb_close(hid->handle);
		return -1;
	}

	pthread_create(&hid->rx_thread, NULL, receive_loop, NULL);

	return 0;
}
#endif
