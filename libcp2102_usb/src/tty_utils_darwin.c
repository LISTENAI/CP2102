#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <stdint.h>

#include "tty_utils.h"

#define LOG_TAG "tty_utils"
#include "log.h"

const char *kUSBSerialNumberString = "USB Serial Number";

static bool
get_string_property(io_object_t device, const char *property, char *buf, size_t limit)
{
	CFTypeRef container = IORegistryEntryCreateCFProperty(device,
		CFStringCreateWithCString(kCFAllocatorDefault, property, kCFStringEncodingUTF8),
		kCFAllocatorDefault, 0);

	if (container) {
		bool success = CFStringGetCString(container, buf, limit, kCFStringEncodingUTF8);
		CFRelease(container);
		return success;
	} else {
		return false;
	}
}

static bool
GetParentDeviceByType(io_object_t device, const char *type, io_object_t *parent)
{
	io_name_t name;
	while (true) {
		if (IOObjectGetClass(device, name) != KERN_SUCCESS) {
			return false;
		}

		if (strcmp(name, type) == 0) {
			*parent = device;
			return true;
		}

		if (IORegistryEntryGetParentEntry(device, kIOServicePlane, &device) != KERN_SUCCESS) {
			return false;
		}
	}

	return false;
}

bool
tty_get_serial(const char *path, char *serial, ssize_t serial_len)
{
	io_name_t name;
	io_object_t usb_device;
	bool success = false;

	io_iterator_t iterator;
	if (IOServiceGetMatchingServices(kIOMainPortDefault, IOServiceMatching("IOSerialBSDClient"),
			&iterator) != KERN_SUCCESS) {
		return false;
	}
	LOGD("enumerating IOSerialBSDClient");
	while (IOIteratorIsValid(iterator)) {
		io_object_t service = IOIteratorNext(iterator);
		if (!service) break;

		memset(name, 0, sizeof(name));
		get_string_property(service, "IOCalloutDevice", name, sizeof(name));
		LOGD("IOCalloutDevice: %s", name);

		if (strcmp(name, path) == 0) {
			if (GetParentDeviceByType(service, "IOUSBHostDevice", &usb_device) ||
				GetParentDeviceByType(service, "IOUSBDevice", &usb_device)) {
				if (get_string_property(usb_device, kUSBSerialNumberString, serial, serial_len)) {
					success = true;
					LOGD("serial number: %s", serial);
				}
			}
			break;
		}
	}
	IOObjectRelease(iterator);

	return success;
}
