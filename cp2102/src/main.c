#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cp2102.h"

#define LOG_TAG "main"
#include "log.h"

static struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'v'},
	{"check", no_argument, NULL, 'c'},
	{"pin", required_argument, NULL, 'p'},
	{"input", no_argument, NULL, 'i'},
	{"output", required_argument, NULL, 'o'},
	{"debug", no_argument, NULL, 0},
	{0, 0, NULL, 0},
};

static const char option_string[] = {"hvcp:io:"};

static struct {
	char *device;
	bool check;
	int pin;
	int input;
	int output;
} options = {
	.device = NULL,
	.check = false,
	.pin = 0,
	.input = 0,
	.output = -1,
};

static void
print_help(const char *progname)
{
	LOGI("Usage: %s <DEVICE> [<OPTIONS>]", basename((char *)progname));
	LOGI("");
	LOGI("Options:");
	LOGI("  -h, --help");
	LOGI("      Print this help message and exit.");
	LOGI("  -v, --version");
	LOGI("      Print version information and exit.");
	LOGI("  -c, --check");
	LOGI("      Check if the device is connected and exit.");
	LOGI("  -p, --pin <PIN>");
	LOGI("      Set the pin to use.");
	LOGI("  -i, --input");
	LOGI("      Read the value from the pin.");
	LOGI("  -o, --output <VALUE>");
	LOGI("      Set the value of the pin.");
}

static void
print_version(void)
{
	LOGI("%s (%d)", GIT_TAG, GIT_INCREMENT);
}

int
main(int argc, char **argv)
{
	set_log_level(LOGLEVEL_INFO);

	int ret = 0;

	int long_index = -1;
	int c;
	while (1) {
		c = getopt_long(argc, argv, option_string, long_options, &long_index);
		if (c == EOF) break;
		switch (c) {
			case 'c':
				options.check = true;
				break;
			case 'p':
				options.pin = atoi(optarg);
				break;
			case 'i':
				options.input = 1;
				break;
			case 'o':
				options.output = atoi(optarg);
				break;
			case 'v':
				print_version();
				goto exit;
			case 0: {
				const char *name = long_options[long_index].name;
				if (strcmp(name, "debug") == 0) {
					set_log_level(LOGLEVEL_DEBUG);
					break;
				}
			}
			case 'h':
			case '?':
				print_help(argv[0]);
				goto exit;
		}
	}

	if (optind >= argc) {
		LOGE("ERROR: Missing device.");
		ret = -1;
		goto exit;
	}

	options.device = argv[optind];

	if (!cp2102_init()) {
		LOGE("Failed to initialize.");
		ret = -1;
		goto exit;
	}

	cp2102_dev_t *dev = cp2102_open(options.device);
	if (options.check) {
		if (dev == NULL) {
			ret = -1;
			goto err_open;
		} else {
			ret = 0;
			goto err_io;
		}
	}
	if (dev == NULL) {
		LOGE("Failed to open device.");
		ret = -1;
		goto err_open;
	}

	if (options.pin == -1) {
		LOGE("Missing pin.");
		ret = -1;
		goto err_io;
	} else if (options.input == 0 && options.output == -1) {
		LOGE("Missing action. (--input or --output)");
		ret = -1;
		goto err_io;
	} else if (options.input == 1 && options.output != -1) {
		LOGE("Cannot set and read at the same time.");
		ret = -1;
		goto err_io;
	}

	if (options.input != 0) {
		bool value;
		if (!cp2102_get_value(dev, options.pin, &value)) {
			LOGE("Failed to read value.");
			ret = -1;
			goto err_io;
		}
		printf("%d\n", value);
	} else if (options.output != -1) {
		if (!cp2102_set_value(dev, options.pin, options.output)) {
			LOGE("Failed to set value.");
			ret = -1;
			goto err_io;
		}
	}

err_io:
	cp2102_close(&dev);
err_open:
	cp2102_exit();
exit:
	return ret;
}
