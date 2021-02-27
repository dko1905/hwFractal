#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "./util/printer.h"

#include "application.h"

/* ## Types ## */
struct Application {
	struct Config *config;
	/* GLFW */
	GLFWwindow *window;
	/* Vulkan */
	VkInstance instance;
};

/* ## Static/private functions ## */
static int window_init(struct Application *app) {
	if (glfwInit() != GLFW_TRUE) {
		perr("Failed to `glfwInit()`: %s", strerror(errno));
		goto init_err;
	}

	/* Disable OpenGL API. */
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	/* Disable resizeing. */
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	/* Create OpenGL window. */
	app->window = glfwCreateWindow(app->config->width, app->config->height, app->config->title, NULL, NULL);
	if (app->window == NULL) {
		perr("Failed to open window: %s", strerror(errno));
		goto window_err;
	}

	return 0;

	glfwDestroyWindow(app->window);
window_err:
	glfwTerminate();
init_err:
	return -1;
}
static int vulkan_init(struct Application *app) {
	VkApplicationInfo app_info = {0};
	VkInstanceCreateInfo create_info = {0};
	VkResult ret = 0;
	/* Extensions */
	uint32_t requested_extensions_found = 0;
	uint32_t required_extensions_found = 0;
	uint32_t glfw_extension_count = 0;
	uint32_t vulkan_extension_count = 0;
	VkExtensionProperties *vulkan_extensions = NULL;
	const char** glfw_extensions = NULL;
	/* Validation layers. */
	uint32_t layers_found = 0;
	uint32_t layer_count = 0;
	VkLayerProperties *layers = NULL;

	/* Check if Vulkan is supported. */
	if (glfwVulkanSupported() == GLFW_FALSE) {
		perr("Vulkan IS NOT supported");
		return -1;
	} else {
		pdebug("Vulkan IS supported");
	}

	/* Set info about our program. */
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = app->config->title;
	app_info.applicationVersion = VK_MAKE_VERSION(app->config->version[0], app->config->version[1], app->config->version[2]);
	app_info.pEngineName = "No Engine";
	app_info.engineVersion = VK_MAKE_VERSION(app->config->version[0], app->config->version[1], app->config->version[2]);
	app_info.apiVersion = VK_API_VERSION_1_0;
	/* More info here. */
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	/* Get required extensions from GLFW. */
	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	/* Show required extensions. */
	pdebug("Required extensions:");
	for (uint32_t i = 0; i < glfw_extension_count; ++i) {
		pdebug("    %s", glfw_extensions[i]);
	}
	/* Show requested extensions. */
	pdebug("Requested extensions:");
	for (uint32_t i = 0; i < app->config->requested_extension_count; ++i) {
		pdebug("    %s", app->config->requested_extensions[i]);
	}
	/* Show available extensions. */
	vkEnumerateInstanceExtensionProperties(NULL, &vulkan_extension_count, NULL);
	vulkan_extensions = calloc(vulkan_extension_count, sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(NULL, &vulkan_extension_count, vulkan_extensions);
	pdebug("Available extensions:");
	for (uint32_t i = 0; i < vulkan_extension_count; ++i) {
		pdebug("    %s : %" PRIu32, vulkan_extensions[i].extensionName, vulkan_extensions[i].specVersion);
	}
	/* Compare the required & requested extensions. */
	for (uint32_t i = 0; i < glfw_extension_count; ++i) {
		for (uint32_t n = 0; n < vulkan_extension_count; ++n) {
			if (strcmp(glfw_extensions[i], vulkan_extensions[n].extensionName) == 0) {
				++required_extensions_found;
			}
			if (strcmp(app->config->requested_extensions[i], vulkan_extensions[n].extensionName) == 0) {
				++requested_extensions_found;
			}
		}
	}
	free(vulkan_extensions);
	if (required_extensions_found == glfw_extension_count) {
		pinfo("All required extensions were found");
	} else {
		perr("%" PRIu32 " of %" PRIu32 " required extensions were found, check debug for more details", required_extensions_found, glfw_extension_count);
		return -1;
	}
	if (requested_extensions_found == app->config->requested_extension_count) {
		pinfo("All requested extensions were found");
	} else {
		pwarn("%" PRIu32 " of %" PRIu32 " requested extensions were found, check debug for more details", requested_extensions_found, app->config->requested_extension_count);
	}
	/* Get available layers. */
	if (app->config->requested_validation_layer_count > 0) {
		vkEnumerateInstanceLayerProperties(&layer_count, NULL);
		layers = calloc(layer_count, sizeof(VkLayerProperties));
		vkEnumerateInstanceLayerProperties(&layer_count, layers);
		/* Show requested layers. */
		pdebug("Requested layers:");
		for (uint32_t n = 0; n < app->config->requested_validation_layer_count; ++n) {
			pdebug("    %s", app->config->requested_validation_layers[n]);
		}
		/* Show available layers. */
		pdebug("Available laysers:");
		for (uint32_t n = 0; n < layer_count; ++n) {
			pdebug("    %s : %" PRIu32, layers[n].layerName, layers[n].specVersion);
		}
		/* Check if all are found. */
		for (uint32_t n = 0; n < app->config->requested_validation_layer_count; ++n) {
			for (uint32_t i = 0; i < layer_count; ++i) {
				if (strcmp(app->config->requested_validation_layers[n], layers[i].layerName) == 0) {
					++layers_found;
				}
			}
		}
		if (layers_found == app->config->requested_validation_layer_count) {
			pinfo("%" PRIu32 " of %" PRIu32 " layer(s) found", layers_found, app->config->requested_validation_layer_count);
			create_info.enabledLayerCount = layers_found;
			create_info.ppEnabledLayerNames = app->config->requested_validation_layers;
		} else {
			pwarn("%" PRIu32 " of %" PRIu32 " layer(s) found", layers_found, app->config->requested_validation_layer_count);
		}

		free(layers);
	}

	/* Set extensions. */
	create_info.enabledExtensionCount = glfw_extension_count;
	create_info.ppEnabledExtensionNames = glfw_extensions;
	/* Set layer count. */
	create_info.enabledLayerCount = 0;

	ret = vkCreateInstance(&create_info, NULL, &app->instance);
	if (ret != VK_SUCCESS) {
		perr("Failed to create instance: %i", ret);
		return -1;
	}

	return 0;
}

/* ## Extern/public functions ## */
struct Application *application_init(struct Config *config) {
	struct Application *app = NULL;

	/* Setup application. */
	if ((app = calloc(1, sizeof(struct Application))) == NULL) {
		perr("Failed to allocate Application struct: %s", strerror(errno));
		goto application_err;
	}
	app->config = config;
	/* Init window. */
	if (window_init(app) != 0) {
		perr("Failed to create window");
		goto window_err;
	}
	/* Init vulkan. */
	if (vulkan_init(app) != 0) {
		perr("Failed to init Vulkan");
		goto vulkan_err;
	}

	return app;

	vkDestroyInstance(app->instance, NULL);
vulkan_err:
	glfwDestroyWindow(app->window);
	glfwTerminate();
window_err:
	free(app);
application_err:
	return NULL;
}

void application_free(struct Application *app) {
	if (app == NULL)
		return;
	/* Vulkan */
	vkDestroyInstance(app->instance, NULL);

	/* OpenGL */
	glfwDestroyWindow(app->window);
	glfwTerminate();

	free(app);
}

void application_main(struct Application *app) {
	while (!glfwWindowShouldClose(app->window)) {
		glfwMakeContextCurrent(app->window); /* Just to be sure. */
		glfwPollEvents();
	}
}
