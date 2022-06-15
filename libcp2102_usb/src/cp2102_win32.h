#ifndef __LIB_CP2102_WIN32_H__
#define __LIB_CP2102_WIN32_H__

#include <windows.h>

#include "cp2102.h"

struct _cp2102_dev_t {
	HANDLE handle;
	char *serial_number;
};

#endif  // __LIB_CP2102_WIN32_H__
