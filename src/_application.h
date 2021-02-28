#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <stdint.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct Application {
	struct Config *config;
	/* GLFW */
	GLFWwindow *window;
	/* Vulkan */
	VkInstance instance;
	uint32_t enabled_extension_count;
	const char **enabled_extensions;
	uint32_t enabled_layer_count;
	const char **enabled_layers;
	VkSurfaceKHR surface;
	VkPhysicalDevice physical_device;
	VkDevice device;
	struct {
		uint32_t graphics;
		uint32_t present;
	} queue_family;
	VkQueue graphics_queue;
	VkQueue present_queue;
};

#endif
