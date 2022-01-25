#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>
#include <stdbool.h>

struct Config {
	/* Talloc options: */
	bool logErrorsToStderr;
	uint8_t reportMemoryUsage; // 0 - no, 1 - yes, 2 - full

	/* SDL2 options: */
	const uint32_t width;
	const uint32_t height;
	const char *title;
};

#endif
