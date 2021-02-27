#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>

struct Config {
	const uint32_t width;
	const uint32_t height;
	const char *title;
	const uint8_t version[3]; /* 0, 1, 2 */
	/* Vulkan */
	uint32_t requested_extension_count;
	const char **requested_extensions;
	uint32_t requested_layer_count;
	const char **requested_layers;
};

#endif
