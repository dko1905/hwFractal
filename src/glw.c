#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glw.h"
#include "runtime_config.h"

struct glw_info {
	GLFWwindow *window;
	/* VAO */
	GLuint _vertex_array_id;
};

/**
 * @returns 0: success, -1: memory, -2: glfwInit, -3: create window.
 */
int glw_init(struct glw_info **info_p, const struct runtime_config *config) {
	struct glw_info *info = NULL;
	int status = 0;

	/* Init the info struct. */
	*info_p = malloc(sizeof(struct glw_info));
	if (*info_p == NULL) {
		status = -1;
		goto malloc_err;
	}
	info = *info_p;

	/* Init GLFW. */
	glewExperimental = GL_TRUE;
	if (glfwInit() == GL_FALSE) {
		status = -2;
		goto glfw_err;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config->glfw_version_minor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config->glfw_version_major);
	glfwWindowHint(GLFW_SAMPLES, 1); /* Disable antialiasing. */
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); /* Make MacOS happy. */
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Open window. */
	info->window = glfwCreateWindow(config->width, config->height, config->window_name, NULL, NULL);
	if (info->window == NULL) {
		status = -3;
		goto glfw_window_err;
	}

	/* Initialize GLEW. */
	glfwMakeContextCurrent(info->window);
	glewExperimental = GL_TRUE; /* Needed for core profile 2. */
	if (glewInit() != GLEW_OK) {
		status = -4;
		goto glew_err;
	}
	/* Catch all keys. */
	glfwSetInputMode(info->window, GLFW_STICKY_KEYS, GL_TRUE);
	/* Set background color. */
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f); /* Dark blue*/

glew_err:
	glfwDestroyWindow(info->window);
glfw_window_err:
	glfwTerminate();
glfw_err:
	free(*info_p);
	*info_p = NULL;
malloc_err:
	return status;
}

void glw_free(struct glw_info **info_p) {
	if (info_p != NULL && *info_p != NULL) {
		struct glw_info *info = *info_p;

		glfwDestroyWindow(info->window);
		glfwTerminate();
		free(*info_p);
		*info_p = NULL;
	}
}
