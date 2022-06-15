#include <stdlib.h>

#include "cp2102.h"

bool
cp2102_init(void)
{
	return false;
}

void
cp2102_exit(void)
{
}

cp2102_dev_t *
cp2102_open(const char *dev_name)
{
	return NULL;
}

void
cp2102_close(cp2102_dev_t **dev)
{
}

bool
cp2102_set_value(cp2102_dev_t *dev, uint32_t pin, bool value)
{
	return false;
}

bool
cp2102_get_value(cp2102_dev_t *dev, uint32_t pin, bool *value)
{
	return false;
}
