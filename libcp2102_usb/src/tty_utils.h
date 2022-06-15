#ifndef __LIB_CP2102_TTY_UTILS_H__
#define __LIB_CP2102_TTY_UTILS_H__

#include <stdbool.h>
#include <stdio.h>

bool tty_get_serial(const char *path, char *serial, ssize_t serial_len);

#endif  // __LIB_CP2102_TTY_UTILS_H__
