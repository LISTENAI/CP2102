#ifndef __LIB_CP2102_H__
#define __LIB_CP2102_H__

#include <stdbool.h>
#include <stdint.h>

bool cp2102_init(void);
void cp2102_exit(void);

struct _cp2102_dev_t;
typedef struct _cp2102_dev_t cp2102_dev_t;

cp2102_dev_t *cp2102_open(const char *dev_name);
void cp2102_close(cp2102_dev_t **dev);

bool cp2102_set_value(cp2102_dev_t *dev, uint32_t pin, bool value);
bool cp2102_get_value(cp2102_dev_t *dev, uint32_t pin, bool *value);

#endif  // __LIB_CP2102_H__
