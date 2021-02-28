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

void application_main(struct Application *app) {
	while (!glfwWindowShouldClose(app->window)) {
		glfwMakeContextCurrent(app->window); /* Just to be sure. */
		glfwPollEvents();
	}
}
