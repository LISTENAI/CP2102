#include "cp2102_libusb.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cp2102.h"
#include "libusb.h"
#include "tty_utils.h"

#define LOG_TAG "cp2102_libusb"
#include "log.h"

#define CP2102_VENDOR_ID 0x10c4
#define CP2102_PRODUCT_ID 0xea60

#define REQTYPE_HOST_TO_DEVICE 0x40
#define REQTYPE_DEVICE_TO_HOST 0xc0

#define CP210X_VENDOR_SPECIFIC 0xff

#define CP210X_READ_LATCH 0x00c2
#define CP210X_WRITE_LATCH 0x37e1

#define SERIAL_NUMBER_LEN 256

bool
cp2102_init(void)
{
	return libusb_init(NULL) == 0;
}

void
cp2102_exit(void)
{
	libusb_exit(NULL);
}

static libusb_device_handle *
find_and_open(const char *serial_number)
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

cp2102_dev_t *
cp2102_open(const char *dev_name)
{
	LOGD("device: %s", dev_name);

	char serial[SERIAL_NUMBER_LEN] = {0};
	if (!tty_get_serial(dev_name, serial, sizeof(serial))) {
		return NULL;
	}

	LOGD("serial number: %s", serial);

	libusb_device_handle *handle = find_and_open(serial);
	if (handle == NULL) {
		return NULL;
	}

	LOGD("port opened");

	cp2102_dev_t *dev = malloc(sizeof(cp2102_dev_t));
	dev->handle = handle;

	return dev;
}

void
cp2102_close(cp2102_dev_t **dev)
{
	if (dev == NULL || *dev == NULL) {
		return;
	}
	if ((*dev)->handle != NULL) {
		libusb_close((*dev)->handle);
	}
	free(*dev);
	*dev = NULL;
}

bool
cp2102_set_value(cp2102_dev_t *dev, uint8_t state, uint8_t mask)
{
	uint16_t bits = (state << 8) | mask;

	int ret = libusb_control_transfer(dev->handle, REQTYPE_HOST_TO_DEVICE, CP210X_VENDOR_SPECIFIC,
		CP210X_WRITE_LATCH, bits, NULL, 0, 0);
	LOGD("set gpio: state: 0x%02x, mask: 0x%02x, ret: %d", state, mask, ret);

	return ret == 0;
}

bool
cp2102_get_value(cp2102_dev_t *dev, uint8_t *state)
{
	uint8_t bits = 0;

	int ret = libusb_control_transfer(dev->handle, REQTYPE_DEVICE_TO_HOST, CP210X_VENDOR_SPECIFIC,
		CP210X_READ_LATCH, 0, &bits, 1, 0);
	LOGD("get gpio: ret: %d, state: 0x%02x", ret, bits);

	if (ret == 1) {
		*state = bits;
		return true;
	} else {
		return false;
	}
}
