#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glw.h"
#include "runtime_config.h"

static const float vertex_buffer_data[] = {
	1.f, 1.f, 0.f,
	1.f, -1.f, 0.f,
	-1.f, -1.f, 0.f,
	-1.f, 1.f, 0.f
};

struct glw_info {
	GLFWwindow *window;
	GLuint vertex_array_id;
	GLuint vertex_buffer;
	GLuint program_id;
};

/**
 * @returns 0: success, -1: memory error, -2: io error
 */
static int read_file_contents(const char *path, char **fp_contents) {
	FILE* fp = NULL;
	int fp_size = 0;

	/* Read file into string. */
	if ((fp = fopen(path, "r")) == NULL) {
		return -2;
	}
	if (fseek(fp, 0, SEEK_END) == -1) {
		fclose(fp);
		return -2;
	}
	if ((fp_size = ftell(fp)) == -1) {
		fclose(fp);
		return -2;
	}
	rewind(fp); /* Cannot return an error. */

	*fp_contents = malloc(fp_size + 1); /* +1 for terminator. */
	if (*fp_contents == NULL) {
		*fp_contents = NULL;
		fclose(fp);
		return -1;
	}
	(*fp_contents)[fp_size] = '\0';

	if (fread(*fp_contents, fp_size, 1, fp) != 1) {
		fclose(fp);
		return -2;
	}
	fclose(fp);
	return 0;
}

static long compile_shaders(const char *vertex_path, const char *fragment_path) {
	GLuint vertex_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
	int read_result = 0, compile_result = 0, log_len = 0;
	char *code_str = NULL;

	/* Read vertex shader. */
	if ((read_result = read_file_contents(vertex_path, &code_str)) != 0) {
		if (read_result == -1) {
			fprintf(stderr, "read_file_contents: Failed to allocate memory.\n");
			return -1;
		} else {
			fprintf(stderr, "read_file_contents: %s\n", strerror(errno));
			return -1;
		}
	}
	/* Compile vertex shader*/
	compile_result = 0;
	log_len = 0;

	glShaderSource(vertex_id, 1, ((const char * const *)&code_str), NULL);
	glCompileShader(vertex_id);
	glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &compile_result);
	glGetShaderiv(vertex_id, GL_INFO_LOG_LENGTH, &log_len);

	if (log_len > 0) {
		char *err_msg = malloc(log_len + 1);
		err_msg[log_len] = '\0';
		glGetShaderInfoLog(vertex_id, log_len, NULL, err_msg);
		fprintf(stderr, "Error while checking vertex shader: %s\n", err_msg);
		free(code_str);
		free(err_msg);
		return -1;
	} else {
		printf("Compiled vertex shader.\n");
	}

	free(code_str);

	/* Read fragment shader. */
	if ((read_result = read_file_contents(fragment_path, &code_str)) != 0) {
		if (read_result == -1) {
			fprintf(stderr, "read_file_contents: Failed to allocate memory.\n");
			return -1;
		} else {
			fprintf(stderr, "read_file_contents: %s\n", strerror(errno));
			return -1;
		}
	}
	/* Compile fragment shader*/
	compile_result = 0;
	log_len = 0;

	glShaderSource(fragment_id, 1, ((const char * const *)&code_str), NULL);
	glCompileShader(fragment_id);
	glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &compile_result);
	glGetShaderiv(fragment_id, GL_INFO_LOG_LENGTH, &log_len);

	if (log_len > 0) {
		char *err_msg = malloc(log_len + 1);
		err_msg[log_len] = '\0';
		glGetShaderInfoLog(fragment_id, log_len, NULL, err_msg);
		fprintf(stderr, "Error while checking fragment shader: %s\n", err_msg);
		free(code_str);
		free(err_msg);
		return -1;
	} else {
		printf("Compiled fragment shader.\n");
	}

	free(code_str);

	/* Link program. */
	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vertex_id);
	glAttachShader(program_id, fragment_id);
	glLinkProgram(program_id);

	glGetShaderiv(program_id, GL_LINK_STATUS, &compile_result);
	glGetShaderiv(program_id, GL_INFO_LOG_LENGTH, &log_len);

	if (log_len > 0) {
		char *err_msg = malloc(log_len + 1);
		err_msg[log_len] = '\0';
		glGetShaderInfoLog(program_id, log_len, NULL, err_msg);
		fprintf(stderr, "Error while linking: %s\n", err_msg);
		free(code_str);
		free(err_msg);
		return -1;
	} else {
		printf("Linked program.\n");
	}

	glDetachShader(program_id, vertex_id);
	glDetachShader(program_id, fragment_id);

	glDeleteShader(vertex_id);
	glDeleteShader(fragment_id);

	return (long)program_id;
}

/*  */
int glw_init(struct glw_info **info_p, const struct runtime_config *config) {
	struct glw_info *info = NULL;
	long tmp_program_id = 0;
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
	/* Setup VAO. */
	glGenVertexArrays(1, &info->vertex_array_id);
	glBindVertexArray(info->vertex_array_id);
	/* Compile shaders. */
	if ((tmp_program_id = compile_shaders(config->vertex_path, config->fragment_path)) < 0) {
		status = -5;
		info->program_id = 0;
		fprintf(stderr, "Failed to compile shaders.\n");
		goto compile_shader_err;
	}
	info->program_id = (GLuint)tmp_program_id;
	/* Setup vertexbuffer. */
	glGenBuffers(1, &info->vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, info->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
	/* Setup callbacks. */
	/* TODO */
	/* Setup uniforms. */
	/* TODO */

	return status;

	glDeleteProgram(info->program_id);
compile_shader_err:
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

		glDeleteBuffers(1, &info->vertex_buffer);
		glDeleteVertexArrays(1, &info->vertex_array_id);
		glDeleteProgram(info->program_id);

		glfwDestroyWindow(info->window);
		glfwTerminate();
		free(*info_p);
		*info_p = NULL;
	}
}

void glw_render(const struct glw_info *info, const struct runtime_config *config) {
	glClear(GL_COLOR_BUFFER_BIT);

	glfwSwapBuffers(info->window);
}

int glw_poll(const struct glw_info *info, const struct runtime_config *config) {
	glfwPollEvents();
	int quit = glfwWindowShouldClose(info->window) == GL_TRUE;
	return quit;
}
