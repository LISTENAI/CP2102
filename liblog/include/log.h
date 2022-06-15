#ifndef __LIB_LOG__
#define __LIB_LOG__

#include <stdbool.h>
#include <stdio.h>

#define LOGLEVEL_ERROR 0
#define LOGLEVEL_INFO 1
#define LOGLEVEL_DEBUG 2

extern int _log_level;

void set_log_level(int level);

#define LOGE(format, ...)                                          \
	do {                                                           \
		if (_log_level >= LOGLEVEL_ERROR) {                        \
			fprintf(stderr, "ERROR: " format "\n", ##__VA_ARGS__); \
			fflush(stderr);                                        \
		}                                                          \
	} while (0);

#define LOGI(format, ...)                                \
	do {                                                 \
		if (_log_level >= LOGLEVEL_INFO) {               \
			fprintf(stdout, format "\n", ##__VA_ARGS__); \
			fflush(stdout);                              \
		}                                                \
	} while (0);

#define LOGD(format, ...)                                                                         \
	do {                                                                                          \
		if (_log_level >= LOGLEVEL_DEBUG) {                                                       \
			fprintf(stdout, "\033[0;36m[DEBUG][" LOG_TAG "] " format "\033[0m\n", ##__VA_ARGS__); \
			fflush(stdout);                                                                       \
		}                                                                                         \
	} while (0);

#endif  // __LIB_LOG__
