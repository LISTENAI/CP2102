#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tty_utils.h"

#define LOG_TAG "tty_utils"
#include "log.h"

static bool
read_string(const char *path, char *buf, size_t limit)
{
	FILE *fp = fopen(path, "r");
	if (fp == NULL) {
		return false;
	}

	bool success = fgets(buf, limit, fp) != NULL;
	if (success) {
		size_t len = strlen(buf);
		if (len > 0 && buf[len - 1] == '\n') {
			buf[len - 1] = '\0';
		}
	}

	fclose(fp);
	return success;
}

bool
tty_get_serial(const char *path, char *serial, ssize_t serial_len)
{
	char tmp[PATH_MAX];

	const char *name = basename((char *)path);

	char device_path[PATH_MAX];
	sprintf(tmp, "/sys/class/tty/%s/device", name);
	if (realpath(tmp, device_path) == NULL) {
		return false;
	}
	LOGD("device_path: %s", device_path);

	char subsystem_path[PATH_MAX];
	sprintf(tmp, "/sys/class/tty/%s/device/subsystem", name);
	if (realpath(tmp, subsystem_path) == NULL) {
		return false;
	}
	LOGD("subsystem_path: %s", subsystem_path);

	const char *subsystem = basename(subsystem_path);

	char *usb_interface_path;
	if (strcmp(subsystem, "usb-serial") == 0) {
		usb_interface_path = dirname(device_path);
	} else if (strcmp(subsystem, "usb") == 0) {
		usb_interface_path = device_path;
	} else {
		return false;
	}
	LOGD("usb_interface_path: %s", usb_interface_path);

	const char *usb_device_path = dirname(usb_interface_path);
	LOGD("usb_device_path: %s", usb_device_path);

	sprintf(tmp, "%s/serial", usb_device_path);
	if (!read_string(tmp, serial, serial_len)) {
		return false;
	}
	LOGD("serial: %s", serial);

	return true;
}
