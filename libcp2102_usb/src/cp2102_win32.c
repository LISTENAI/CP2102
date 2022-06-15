#include "cp2102_win32.h"

#include "CP210xRuntimeDLL.h"
#include "cp2102.h"

#define LOG_TAG "cp2102_win32"
#include "log.h"

#define SERIAL_NUMBER_LEN 256

bool
cp2102_init(void)
{
	return true;
}

void
cp2102_exit(void)
{
}

cp2102_dev_t *
cp2102_open(const char *dev_name)
{
	LOGD("device: %s", dev_name);

	HANDLE handle = CreateFile(dev_name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0);

	if (handle == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	LOGD("port opened");

	BYTE partNumber = 0;
	if (CP210xRT_GetPartNumber(handle, &partNumber) != CP210x_SUCCESS) {
		CloseHandle(handle);
		return NULL;
	}
	LOGD("part number: 0x%02x", partNumber);

	CHAR serial[SERIAL_NUMBER_LEN] = {0};
	BYTE serialLength = (BYTE)SERIAL_NUMBER_LEN;
	if (CP210xRT_GetDeviceSerialNumber(handle, serial, &serialLength, true) != CP210x_SUCCESS) {
		CloseHandle(handle);
		return NULL;
	}
	LOGD("serial number: %s", serial);

	cp2102_dev_t *dev = malloc(sizeof(cp2102_dev_t));
	dev->handle = handle;

	dev->serial_number = malloc(strlen(serial) + 1);
	strcpy(dev->serial_number, serial);

	return dev;
}

void
cp2102_close(cp2102_dev_t **dev)
{
	if (dev == NULL || *dev == NULL) {
		return;
	}
	if ((*dev)->handle != NULL) {
		CloseHandle((*dev)->handle);
	}
	if ((*dev)->serial_number != NULL) {
		free((*dev)->serial_number);
	}
	free(*dev);
	*dev = NULL;
}

const char *
cp2102_get_serial_number(cp2102_dev_t *dev)
{
	return (const char *)dev->serial_number;
}

bool
cp2102_set_value(cp2102_dev_t *dev, uint8_t state, uint8_t mask)
{
	CP210x_STATUS ret = CP210xRT_WriteLatch(dev->handle, mask, state);
	LOGD("CP210xRT_WriteLatch: state: 0x%02x, mask: 0x%02x, ret: %d", state, mask, ret);

	return ret == CP210x_SUCCESS;
}

bool
cp2102_get_value(cp2102_dev_t *dev, uint8_t *state)
{
	WORD latch = 0;

	CP210x_STATUS ret = CP210xRT_ReadLatch(dev->handle, &latch);
	LOGD("CP210xRT_ReadLatch: ret: %d, latch: 0x%02x", ret, latch);

	if (ret == CP210x_SUCCESS) {
		*state = latch;
		return true;
	} else {
		return false;
	}
}
