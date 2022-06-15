#include "core.h"

#include <stdlib.h>

#include "cp2102.h"
#include "libusb.h"
#include "tty_utils.h"
#include "usb_utils.h"

#define LOG_TAG "core"
#include "log.h"

#define REQTYPE_HOST_TO_DEVICE 0x40
#define REQTYPE_DEVICE_TO_HOST 0xc0

#define CP210X_VENDOR_SPECIFIC 0xff

#define CP210X_WRITE_LATCH 0x37e1
#define CP210X_READ_LATCH 0x00c2

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

cp2102_dev_t *
cp2102_open(const char *dev_name)
{
	LOGD("device: %s", dev_name);

	char serial[SERIAL_NUMBER_LEN] = {0};
	if (!tty_get_serial(dev_name, serial, sizeof(serial))) {
		return NULL;
	}

	LOGD("serial number: %s", serial);

	libusb_device_handle *handle = libusb_find_and_open(serial);
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
cp2102_set_value(cp2102_dev_t *dev, uint32_t pin, bool value)
{
	uint8_t state = value ? (1 << pin) : 0;
	uint8_t mask = 1 << pin;

	uint16_t bits = (state << 8) | mask;

	int ret = libusb_control_transfer(dev->handle, REQTYPE_HOST_TO_DEVICE, CP210X_VENDOR_SPECIFIC,
		CP210X_WRITE_LATCH, bits, NULL, 0, 0);
	LOGD("set gpio %d to %d, bits: 0x%04x, ret: %d", pin, value, bits, ret);

	return ret == 0;
}

bool
cp2102_get_value(cp2102_dev_t *dev, uint32_t pin, bool *value)
{
	uint8_t bits = 0;

	int ret = libusb_control_transfer(dev->handle, REQTYPE_DEVICE_TO_HOST, CP210X_VENDOR_SPECIFIC,
		CP210X_READ_LATCH, 0, &bits, 1, 0);
	LOGD("get gpio %d, ret: %d, bits: 0x%02x", pin, ret, bits);

	if (ret == 1) {
		*value = bits & (1 << pin);
	}
	return ret == 1;
}
