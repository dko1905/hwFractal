#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "./util/printer.h"

#include "application.h"
#include "_application.h" /* For "private" types. */

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
	const char **required_extensions = NULL;
	uint32_t required_extension_count = 0;
	const char **requested_extensions = NULL;
	uint32_t requested_extension_count = 0;
	const char **enabled_extensions = NULL;
	uint32_t enabled_extension_count = 0;
	VkExtensionProperties *available_extensions = NULL;
	uint32_t available_extension_count = 0;
	/* Layers */
	const char **required_layers = NULL;
	uint32_t required_layer_count = 0;
	const char **requested_layers = NULL;
	uint32_t requested_layer_count = 0;
	const char **enabled_layers = NULL;
	uint32_t enabled_layer_count = 0;
	VkLayerProperties *available_layers = NULL;
	uint32_t available_layer_count = 0;

	/* Check for Vulkan support. */
	if (glfwVulkanSupported() == GLFW_TRUE) {
		pinfo("Vulkan support found");
	} else {
		perr("Vulkan support not found");
		goto support_err;
	}
	/* Fill structs with data about our program. */
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = app->config->title;
	app_info.applicationVersion = VK_MAKE_VERSION(app->config->version[0], app->config->version[1], app->config->version[2]);
	app_info.pEngineName = "No Engine";
	app_info.engineVersion = VK_MAKE_VERSION(app->config->version[0], app->config->version[1], app->config->version[2]);
	app_info.apiVersion = VK_API_VERSION_1_0;

	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	/* ### Extensions ### */
	/* Get the required extensions. */
	required_extensions = glfwGetRequiredInstanceExtensions(&required_extension_count);
	if (required_extensions == NULL) {
		perr("Vulkan cannot render to screen");
		goto get_ext_err;
	}
	/* Get requested extensions. */
	requested_extensions = app->config->requested_extensions;
	requested_extension_count = app->config->requested_extension_count;
	/* Get available extensions. */
	ret = vkEnumerateInstanceExtensionProperties(NULL, &available_extension_count, NULL);
	if (ret != VK_SUCCESS) {
		perr("Failed to get number of extensions: %u", ret);
		goto get_ext_err;
	}
	available_extensions = calloc(available_extension_count, sizeof(*available_extensions));
	ret = vkEnumerateInstanceExtensionProperties(NULL, &available_extension_count, available_extensions);
	if (ret != VK_SUCCESS) {
		perr("Failed to get extensions: %u", ret);
		goto get_ext_err;
	}
	/* Print required/requested/available extensions. */
	pdebug("Required extensions:");
	for (uint32_t n = 0; n < required_extension_count; ++n) {
		pdebug("    %s", required_extensions[n]);
	}
	pdebug("Requested extensions:");
	for (uint32_t n = 0; n < requested_extension_count; ++n) {
		pdebug("    %s", requested_extensions[n]);
	}
	pdebug("Available extensions:");
	for (uint32_t n = 0; n < available_extension_count; ++n) {
		pdebug("    %s", available_extensions[n].extensionName);
	}
	/* Check if they are available. */
	{ /* Create new scope. */
		uint32_t required_extensions_found = 0, requested_extensions_found = 0, enabled_extensions_free = 0;
		/* Check if extensions are available. */
		for (uint32_t n = 0; n < required_extension_count; ++n) {
			for (uint32_t i = 0; i < available_extension_count; ++i) {
				if (strcmp(required_extensions[n], available_extensions[i].extensionName) == 0) {
					++required_extensions_found;
				}
			}
		}
		for (uint32_t n = 0; n < requested_extension_count; ++n) {
			for (uint32_t i = 0; i < available_extension_count; ++i) {
				if (strcmp(requested_extensions[n], available_extensions[i].extensionName) == 0) {
					++requested_extensions_found;
				}
			}
		}
		/* Print status. */
		if (required_extensions_found == required_extension_count) {
			pinfo("Found %" PRIu32 " of %" PRIu32 " required extensions", required_extensions_found, required_extension_count);
		} else {
			perr("Found %" PRIu32 " of %" PRIu32 " required extensions", required_extensions_found, required_extension_count);
			goto set_ext_err;
		}
		if (requested_extensions_found == requested_extension_count) {
			pinfo("Found %" PRIu32 " of %" PRIu32 " requested extensions", requested_extensions_found, requested_extension_count);
		} else {
			pwarn("Found %" PRIu32 " of %" PRIu32 " requested extensions", requested_extensions_found, requested_extension_count);
		}
		/* Allocate place for extensions. */
		enabled_extension_count = required_extensions_found + requested_extensions_found;
		enabled_extensions = calloc(enabled_extension_count, sizeof(char *));
		/* Put enabled extensions into array. */
		for (uint32_t n = 0; n < required_extension_count; ++n) {
			for (uint32_t i = 0; i < available_extension_count; ++i) {
				if (strcmp(required_extensions[n], available_extensions[i].extensionName) == 0) {
					enabled_extensions[enabled_extensions_free++] = required_extensions[n];
				}
			}
		}
		for (uint32_t n = 0; n < requested_extension_count; ++n) {
			for (uint32_t i = 0; i < available_extension_count; ++i) {
				if (strcmp(requested_extensions[n], available_extensions[i].extensionName) == 0) {
					enabled_extensions[enabled_extensions_free++] = requested_extensions[n];
				}
			}
		}
		/* Enable these extensions. */
		create_info.enabledExtensionCount = enabled_extension_count;
		create_info.ppEnabledExtensionNames = enabled_extensions;
		app->enabled_extensions = enabled_extensions;
		app->enabled_extension_count = enabled_extension_count;
		/* Print currently enabled extensions. */
		pdebug("Enabled extensions:");
		for (uint32_t n = 0; n < enabled_extension_count; ++n) {
			pdebug("    %s", enabled_extensions[n]);
		}
	}
	/* ### Layers ### */
	/* Get required layers. */
	required_layers = NULL; /* No layers are required. */
	required_layer_count = 0;
	/* Get requested layers. */
	requested_layers = app->config->requested_layers;
	requested_layer_count = app->config->requested_layer_count;
	/* Get available layers. */
	ret = vkEnumerateInstanceLayerProperties(&available_layer_count, NULL);
	if (ret != VK_SUCCESS) {
		perr("Failed to get number of layers: %i", ret);
		goto get_lay_err;
	}
	available_layers = calloc(available_layer_count, sizeof(*available_layers));
	ret = vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers);
	if (ret != VK_SUCCESS) {
		perr("Failed to get layers: %i", ret);
		goto get_lay_err;
	}
	/* Print required/requested/available layers. */
	pdebug("Required layers:");
	for (uint32_t n = 0; n < required_layer_count; ++n) {
		pdebug("    %s", required_layers[n]);
	}
	pdebug("Requested layers:");
	for (uint32_t n = 0; n < requested_layer_count; ++n) {
		pdebug("    %s", requested_layers[n]);
	}
	pdebug("Available layers:");
	for (uint32_t n = 0; n < available_layer_count; ++n) {
		pdebug("    %s", available_layers[n].layerName);
	}
	/* Check if they are available. */
	{ /* New scope. */
		uint32_t required_layers_found = 0, requested_layers_found = 0, enabled_layers_free = 0;
		for (uint32_t n = 0; n < required_layer_count; ++n) {
			for (uint32_t i = 0; i < available_layer_count; ++i) {
				if (strcmp(required_layers[n], available_layers[i].layerName) == 0) {
					++required_layers_found;
				}
			}
		}
		for (uint32_t n = 0; n < requested_layer_count; ++n) {
			for (uint32_t i = 0; i < available_layer_count; ++i) {
				if (strcmp(requested_layers[n], available_layers[i].layerName) == 0) {
					++requested_layers_found;
				}
			}
		}
		/* Print status. */
		if (required_layers_found == required_layer_count) {
			pinfo("Found %" PRIu32 " of %" PRIu32 " required layers", required_layers_found, required_layer_count);
		} else {
			perr("Found %" PRIu32 " of %" PRIu32 " required layers", required_layers_found, required_layer_count);
			goto set_lay_err;
		}
		if (requested_layers_found == requested_layer_count) {
			pinfo("Found %" PRIu32 " of %" PRIu32 " requested layers", requested_layers_found, requested_layer_count);
		} else {
			pwarn("Found %" PRIu32 " of %" PRIu32 " requested layers", requested_layers_found, requested_layer_count);
		}
		/* Allocate space for layers. */
		enabled_layer_count = required_layers_found + requested_layers_found;
		enabled_layers = calloc(enabled_layer_count, sizeof(*enabled_layers));
		/* Put enabled layers. */
		for (uint32_t n = 0; n < required_layer_count; ++n) {
			for (uint32_t i = 0; i < available_layer_count; ++i) {
				if (strcmp(required_layers[n], available_layers[i].layerName) == 0) {
					enabled_layers[enabled_layers_free++] = available_layers[i].layerName;
				}
			}
		}
		for (uint32_t n = 0; n < requested_layer_count; ++n) {
			for (uint32_t i = 0; i < available_layer_count; ++i) {
				if (strcmp(requested_layers[n], available_layers[i].layerName) == 0) {
					enabled_layers[enabled_layers_free++] = available_layers[i].layerName;
				}
			}
		}
		/* Enabled layers. */
		create_info.enabledLayerCount = enabled_layer_count;
		create_info.ppEnabledLayerNames = enabled_layers;
		app->enabled_layers = enabled_layers;
		app->enabled_layer_count = enabled_layer_count;
		/* Print enabled layers. */
		pdebug("Enabled layers:");
		for (uint32_t n = 0; n < enabled_layer_count; ++n) {
			pdebug("    %s", enabled_layers[n]);
		}
	}

	ret = vkCreateInstance(&create_info, NULL, &app->instance);
	if (ret != VK_SUCCESS) {
		perr("Failed to create Vulkan instance: %i", ret);
		goto instance_err;
	} else {
		pdebug("Created instance");
	}

	free(available_layers);
	free(available_extensions);
	return 0;

	vkDestroyInstance(app->instance, NULL);
instance_err:
set_lay_err:
	free(available_layers);
get_lay_err:
set_ext_err:
	free(available_extensions);
get_ext_err:
support_err:
	return -1;
}
static int create_surface(struct Application *app) {
	VkResult ret = VK_SUCCESS;

	ret = glfwCreateWindowSurface(app->instance, app->window, NULL, &app->surface);
	if (ret != VK_SUCCESS) {
		perr("Failed to create window surface: %i", ret);
		return -1;
	} else {
		pdebug("Created surface");
	}

	return 0;
}
struct DeviceFeatures {
	char device_name[256];
	bool geometry_shader_supported;
	bool shader_float64_supported;
	struct {
		int32_t graphics;
		int32_t present;
	} queue_family;
};
static void get_device_features(struct Application *app, VkPhysicalDevice device, struct DeviceFeatures *df) {
	VkPhysicalDeviceProperties properties = {0};
	VkPhysicalDeviceFeatures features = {0};
	uint32_t queue_property_count = 0;
	VkBool32 present_bool = VK_FALSE;
	VkQueueFamilyProperties *queue_properties = NULL;

	/* Get data from Vulkan. */
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_property_count, NULL);
	queue_properties = calloc(queue_property_count, sizeof(*queue_properties));
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_property_count, queue_properties);

	/* Insert data into struct. */
	strncpy(df->device_name, properties.deviceName, 256);
	if (features.geometryShader == VK_TRUE) {
		df->geometry_shader_supported = true;
	} else {
		df->geometry_shader_supported = false;
	}
	if (features.shaderFloat64 == VK_TRUE) {
		df->shader_float64_supported = true;
	} else {
		df->shader_float64_supported = false;
	}
	/* Find the corrent queues. */
	df->queue_family.graphics = -1;
	df->queue_family.present = -1;
	for (uint32_t n = 0; n < queue_property_count; ++n) {
		vkGetPhysicalDeviceSurfaceSupportKHR(device, n, app->surface, &present_bool);
		if (queue_properties[n].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			df->queue_family.graphics = n;
		}
		if (present_bool == VK_TRUE) {
			df->queue_family.present = n;
		}
	}

	free(queue_properties);
}
static int pick_physical_device(struct Application *app) {
	VkResult ret = 0;
	uint32_t device_count = 0;
	VkPhysicalDevice *devices = NULL;
	struct DeviceFeatures device_features = {0};

	/* Get devices. */
	ret = vkEnumeratePhysicalDevices(app->instance, &device_count, NULL);
	if (ret != VK_SUCCESS) {
		perr("Failed to get number of devices: %i", ret);
		goto get_device;
	}
	devices = calloc(device_count, sizeof(*devices));
	ret = vkEnumeratePhysicalDevices(app->instance, &device_count, devices);
	if (ret != VK_SUCCESS) {
		perr("Failed to get devices: %i", ret);
		goto gen_device;
	} else {
		pdebug("Found %" PRIu32 " device(s)", device_count);
	}
	if (device_count < 1) {
		perr("No device found");
		goto gen_device;
	}
	/* Print devices. */
	pinfo("Devices:");
	for (uint32_t n = 0; n < device_count; ++n) {
		get_device_features(app, devices[n], &device_features);

		pinfo("    %s", device_features.device_name);
		pinfo("        Geometry shader: %" PRIu32, device_features.geometry_shader_supported);
		pinfo("        Float64: %" PRIu32, device_features.shader_float64_supported);
		pinfo("        Queue:");
		pinfo("            Graphics: %" PRIi32, device_features.queue_family.graphics);
		pinfo("            Present: %" PRIi32, device_features.queue_family.present);
	}
	/* Pick device. */
	if (app->config->device_pick != -1) {
		if (app->config->device_pick < device_count && app->config->device_pick >= 0) {
			app->physical_device = devices[app->config->device_pick];
		} else {
			perr("Device selected is invalid");
			goto gen_device;
		}
	} else {
		do {
			pinfo("Please select a device (0-%" PRIu32 ")", device_count - 1);
			scanf("%" PRIu32, &app->config->device_pick);
			if (app->config->device_pick < device_count && app->config->device_pick >= 0) {
				app->physical_device = devices[app->config->device_pick];
				break;
			} else {
				pwarn("Device selected is invalid");
			}
		} while(1);
	}
	pinfo("Selected device %" PRIu32, app->config->device_pick);
	/* Save data into `Application` struct. */
	get_device_features(app, app->physical_device, &device_features);
	app->queue_family.graphics = device_features.queue_family.graphics;
	app->queue_family.present = device_features.queue_family.present;
	/* Done. */
	free(devices);
	return 0;

gen_device:
	free(devices);
get_device:
	return -1;
}
static int create_logical_device(struct Application *app) {
	VkResult ret = VK_SUCCESS;
	VkDeviceQueueCreateInfo *queue_creat_info_arr = NULL;
	VkPhysicalDeviceFeatures device_features = {0};
	VkDeviceCreateInfo creat_info = {0};
	float queuePriority = 1.0f;
	uint32_t actual_queue_count = 0;

	{
		const uint32_t queue_count = sizeof(app->queue_family)/sizeof(uint32_t);
		uint32_t *prev_queue = calloc(queue_count, sizeof(*prev_queue));
		queue_creat_info_arr = calloc(queue_count, sizeof(*queue_creat_info_arr));
		uint32_t prev_queue_count = 0;
		for (uint32_t n = 0; n < queue_count; ++n) {
			const uint32_t index = ((uint32_t*)&(app->queue_family))[n];
			/* Check for duplicates. */
			for (uint32_t i = 0; i < prev_queue_count; ++i) {
				if (prev_queue[i] == index) {
					goto continue_outer;
				}
			}
			prev_queue[prev_queue_count++] = index;
			++actual_queue_count;

			/* Fill info. */
			VkDeviceQueueCreateInfo queue_creat_info = {0};
			queue_creat_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    		queue_creat_info.queueFamilyIndex = ((uint32_t*)&(app->queue_family))[n];
    		queue_creat_info.queueCount = 1;
    		queue_creat_info.pQueuePriorities = &queuePriority;
			queue_creat_info_arr[n] = queue_creat_info;

		continue_outer:
			continue; /* Not actually needed. */
		}
		free(prev_queue);
	}
	pdebug("Created %" PRIu32 " queues", actual_queue_count);

	creat_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	creat_info.pQueueCreateInfos = queue_creat_info_arr;
	creat_info.queueCreateInfoCount = actual_queue_count;

	creat_info.pEnabledFeatures = &device_features;
	creat_info.enabledExtensionCount = 0;
	creat_info.enabledLayerCount = app->enabled_layer_count;
	creat_info.ppEnabledLayerNames = app->enabled_layers;

	ret = vkCreateDevice(app->physical_device, &creat_info, NULL, &app->device);
	free(queue_creat_info_arr);
	if (ret != VK_SUCCESS) {
		perr("Failed to create device: %i", ret);
		return -1;
	} else {
		pdebug("Created logical device");
	}

	vkGetDeviceQueue(app->device, app->queue_family.graphics, 0, &app->graphics_queue);
	vkGetDeviceQueue(app->device, app->queue_family.present, 0, &app->present_queue);
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
	/* Create drawing surface. */
	if (create_surface(app) != 0) {
		perr("Failed to create surface");
		goto surface_err;
	}
	/* Pick physical device. */
	if (pick_physical_device(app) != 0) {
		perr("Failed to pick physical device");
		goto phys_device_err;
	}
	/* Setup logical device. */
	if (create_logical_device(app) != 0) {
		perr("Failed to create logical device");
		goto logi_device_err;
	}

	return app;

logi_device_err:
phys_device_err:
	vkDestroySurfaceKHR(app->instance, app->surface, NULL);
surface_err:
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
	/* Queues are auto removed. */
	vkDestroyDevice(app->device, NULL);
	/* Physical device is auto removed. */
	vkDestroySurfaceKHR(app->instance, app->surface, NULL);
	vkDestroyInstance(app->instance, NULL);
	free(app->enabled_extensions);
	free(app->enabled_layers);

	/* OpenGL */
	glfwDestroyWindow(app->window);
	glfwTerminate();

	free(app);
}
