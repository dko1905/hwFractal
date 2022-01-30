#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>
#include <stdbool.h>

struct Config {
	/* Talloc options: */
	bool talloc_log_err;
	uint8_t talloc_report_mem_usage; // 0 - no, 1 - yes, 2 - full

	/* SDL2 options: */
	uint32_t width;
	uint32_t height;
	char *title;
	uint32_t fps;
	uint64_t update_timeout;
};

#endif
