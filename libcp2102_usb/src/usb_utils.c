#include "usb_utils.h"

#include <stdint.h>
#include <string.h>

#include "libusb.h"

#define LOG_TAG "usb_utils"
#include "log.h"

#define CP2102_VENDOR_ID 0x10c4
#define CP2102_PRODUCT_ID 0xea60

libusb_device_handle *
libusb_find_and_open(const char *serial_number)
{
	ssize_t count = 0;
	libusb_device **devices;

	if ((count = libusb_get_device_list(NULL, &devices)) < 0) {
		return NULL;
	}

	LOGD("enumerating %zd devices", count);

	libusb_device_handle *found = NULL;

	libusb_device_handle *handle = NULL;
	struct libusb_device_descriptor desc = {0};
	char serial[SERIAL_NUMBER_LEN] = {0};

	for (ssize_t i = 0; i < count; i++) {
		if (libusb_get_device_descriptor(devices[i], &desc) < 0) {
			continue;
		}

		LOGD("device: %04x:%04x", desc.idVendor, desc.idProduct);

		if (desc.idVendor != CP2102_VENDOR_ID || desc.idProduct != CP2102_PRODUCT_ID) {
			continue;
		}

		if (libusb_open(devices[i], &handle) < 0) {
			continue;
		}

		if (libusb_get_string_descriptor_ascii(
				handle, desc.iSerialNumber, (unsigned char *)serial, sizeof(serial)) > 0) {
			LOGD("serial number: %s", serial);
			if (strcmp((const char *)serial, serial_number) == 0) {
				found = handle;
			}
		}

		if (found != NULL) {
			break;
		} else {
			libusb_close(handle);
		}
	}

	LOGD("found device: %d", found != NULL);

	libusb_free_device_list(devices, 1);
	return found;
}
