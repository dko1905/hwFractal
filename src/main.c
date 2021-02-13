#include <stdio.h>
#include <stdlib.h>

#include "glw.h"
#include "runtime_config.h"

int main() {
	struct glw_info *info = NULL;
	struct runtime_config config;
	int glw_result = 0;

	config.window_name = "Test window";
	config.width = 600;
	config.height = 600;
	config.glfw_version_major = 3;
	config.glfw_version_minor = 3;

	if ((glw_result = glw_init(&info, &config)) < 0) {
		fprintf(stderr, "Failed to init OpenGL: %i\n", glw_result);
		return EXIT_FAILURE;
	}
	printf("It went good.\n");

	scanf("\n"); /* Wait until newline. */

	glw_free(&info);

	return EXIT_SUCCESS;
}
