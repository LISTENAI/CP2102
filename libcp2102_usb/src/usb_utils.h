#ifndef __LIB_CP2102_USB_UTILS_H__
#define __LIB_CP2102_USB_UTILS_H__

#include "libusb.h"

#define SERIAL_NUMBER_LEN 256

libusb_device_handle *libusb_find_and_open(const char *serial_number);

#endif  // __LIB_CP2102_USB_UTILS_H__
