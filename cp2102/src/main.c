#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cp2102.h"

#define LOG_TAG "main"
#include "log.h"

#define CP2102_PORTS 7

static struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'v'},
	{"check", no_argument, NULL, 'c'},
	{"debug", no_argument, NULL, 0},
	{0, 0, NULL, 0},
};

static const char option_string[] = {
	"hvc"
	"0::"
	"1::"
	"2::"
	"3::"
	"4::"
	"5::"
	"6::",
};

static struct {
	char *device;
	bool check;
	uint8_t write_mask;
	uint8_t write_bits;
	uint8_t read_mask;
	uint8_t output_offsets[CP2102_PORTS];
} options = {
	.device = NULL,
	.check = false,
	.write_mask = 0,
	.write_bits = 0,
	.read_mask = 0,
	.output_offsets = {0},
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
	LOGI("  -0[X] .. -6[X]");
	LOGI("      Read GPIO[0..6] state, optionally set them to X.");
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
	int output_idx = 0;

	int long_index = -1;
	int c;
	while (1) {
		c = getopt_long(argc, argv, option_string, long_options, &long_index);
		if (c == EOF) break;

		if (c >= '0' && c <= '6') {
			uint8_t gpio = c - '0';
			uint8_t state;

			if (options.read_mask & (1 << gpio)) {
				LOGE("Redundant read of GPIO%d", gpio);
				ret = -1;
				goto exit;
			}

			options.read_mask |= 1 << gpio;
			options.output_offsets[gpio] = output_idx++;

			if (optarg != NULL) {
				state = atoi(optarg) ? 1 : 0;
				LOGD("set gpio %d to %d", gpio, state);
				options.write_mask |= 1 << gpio;
				options.write_bits |= state << gpio;
			} else {
				LOGD("get gpio %d", gpio);
			}

			continue;
		}

		switch (c) {
			case 'c':
				options.check = true;
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
		LOGE("Missing device.");
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

	LOGD("write_mask: 0x%02x", options.write_mask);
	LOGD("write_bits: 0x%02x", options.write_bits);

	if (options.write_mask) {
		if (!cp2102_set_value(dev, options.write_bits, options.write_mask)) {
			LOGE("Failed to write to device.");
			ret = -1;
			goto err_io;
		}
	}

	LOGD("read_mask: 0x%02x", options.read_mask);

	if (options.read_mask) {
		uint8_t state;
		char output[CP2102_PORTS + 1] = {0};
		if (!cp2102_get_value(dev, &state)) {
			LOGE("Failed to read from device.");
			ret = -1;
			goto err_io;
		}
		for (int i = 0; i < CP2102_PORTS; i++) {
			if (options.read_mask & (1 << i)) {
				if (state & (1 << i)) {
					output[options.output_offsets[i]] = '1';
				} else {
					output[options.output_offsets[i]] = '0';
				}
			}
		}
		printf("%s\n", output);
	}

err_io:
	cp2102_close(&dev);
err_open:
	cp2102_exit();
exit:
	return ret;
}
