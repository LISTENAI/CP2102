#ifndef __LIB_CP2102_LIBUSB_H__
#define __LIB_CP2102_LIBUSB_H__

#include "cp2102.h"
#include "libusb.h"

struct _cp2102_dev_t {
	libusb_device_handle *handle;
	char *serial_number;
};

#endif  // __LIB_CP2102_LIBUSB_H__
