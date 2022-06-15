#ifndef __LIB_CP2102_CORE_H__
#define __LIB_CP2102_CORE_H__

#include "cp2102.h"
#include "libusb.h"

struct _cp2102_dev_t {
	libusb_device_handle *handle;
};

#endif  // __LIB_CP2102_CORE_H__
