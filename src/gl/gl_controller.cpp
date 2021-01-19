#include <memory>
#include <iostream>
#include <string>

#include "exceptions/runtime_exception.hpp"
#include "interfaces/core_controller.hpp"
#include "util/printer.hpp"
#include "shaders.hpp"

#include "gl_controller.hpp"

using namespace hwfractal;
using namespace gl;

static std::map<int, bool> *keysdown = NULL;
static glm::vec2 *u_resolution;

gl_controller::gl_controller(const std::shared_ptr<const config> &config) : core_controller(config) {
	this->_config = config;

	const glm::vec2 resolution = {600.f, 400.f};

	glewExperimental = true; /* Needed for core profile. */
	if (!glfwInit()) {
		throw runtime_exception("Failed to initialize GLFW");
	}
	/* Request OpenGL 4.1 or 3.3 depending on GL_USE_DOUBLE. */
	if (this->_use_double) {
		printer::info("Using GLFW version 4.1");
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	} else {
		printer::info("Using GLFW version 3.3");
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	}
	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); /* Make MacOS happy. */
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	/* Open window. */
	this->_window = glfwCreateWindow(resolution.x, resolution.y, "hwfractal", NULL, NULL);
	if (this->_window == NULL) {
		glfwTerminate(); /* Cleanup. */
		this->_window = NULL;
		throw runtime_exception("Failed to open window");
	}
	/* Initialize GLEW. */
	glfwMakeContextCurrent(this->_window);
	glewExperimental = true; /* Needed for core profile 2. */
	if (glewInit() != GLEW_OK) {
		glfwTerminate(); /* Cleanup. */
		this->_window = NULL;
		throw runtime_exception("Failed to initialize GLEW");
	}
	/* Catch all keys, aka ESC. */
	glfwSetInputMode(this->_window, GLFW_STICKY_KEYS, GL_TRUE);
	/* Dark blue clear color. */
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	/* Setup VAO. */
	glGenVertexArrays(1, &this->_vertex_array_id);
	glBindVertexArray(this->_vertex_array_id);
	/* Compile shaders. */
	this->_program_id = shaders::compile(
		this->_config->get("GL_SHADER_VPATH"),
		this->_use_double ? this->_config->get("GL_SHADER_FPATH_DOUBLE") : this->_config->get("GL_SHADER_FPATH") 
	);
	/* Setup vertexbuffer. */
	glGenBuffers(1, &this->_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gl::vertex_buffer_data), gl::vertex_buffer_data, GL_STATIC_DRAW);
	/* Setup callbacks. */
	GLFWframebuffersizefun resisze_lamda = [](GLFWwindow* _w, int width, int height) {
		glViewport(0, 0, width, height);
		*u_resolution = glm::vec2(width, height);
	};
	auto key_lamda = [](GLFWwindow* _w, int key, int _scancode, int action, int _mods) {
		if(action == GLFW_PRESS || action == GLFW_RELEASE){
			if(action == GLFW_PRESS){
				(*keysdown)[key] = true;
			} else{
				(*keysdown)[key] = false;
			}
		}
	};
	keysdown = &this->_keysdown;
	u_resolution = &this->_u_resolution;
	glfwSetFramebufferSizeCallback(this->_window, resisze_lamda);
	glfwSetKeyCallback(this->_window, key_lamda);
	/* Setup uniforms. */
	if (this->_use_double) {
		printer::info("Shader using 64-bit floats.");
	} else {
		printer::info("Shader using 32-bit floats.");
	}
	this->_u_resolution = resolution;

	this->_u_iter_max_id = glGetUniformLocation(this->_program_id, "u_iter_max");
	this->_u_scale_id = glGetUniformLocation(this->_program_id, "u_scale");
	this->_u_pan_id = glGetUniformLocation(this->_program_id, "u_pan");
	this->_u_resolution_id = glGetUniformLocation(this->_program_id, "u_resolution");
	if (this->_u_scale_id < 0 || this->_u_pan_id < 0 || this->_u_resolution_id < 0) {
		throw runtime_exception("Failed to find uniform, continueing program.");
	}
}

gl_controller::~gl_controller() {
	/* Cleanup any resources used by OpenGL. */
	if (this->_window != NULL) {
		/* Remove callbacks. */
		glfwSetKeyCallback(this->_window, NULL);
		glfwSetFramebufferSizeCallback(this->_window, NULL);
		/* Cleanup VBO. */
		glDeleteBuffers(1, &this->_vertexbuffer);
		glDeleteVertexArrays(1, &this->_vertex_array_id);
		glDeleteProgram(this->_program_id);
		/* Cleanup the rest. */
		glfwTerminate();
	}
}

void gl_controller::render() const {
	glClear(GL_COLOR_BUFFER_BIT);

	/* Use shader. */
	glUseProgram(this->_program_id);

	/* Set uniform, might need to be into init. */
	glUniform2fv(this->_u_resolution_id, 1, &this->_u_resolution[0]);
	glUniform1i(this->_u_iter_max_id, this->_u_iter_max);
	if (this->_use_double) {
		glUniform1d(this->_u_scale_id, this->_u_scale_d);
		glUniform2dv(this->_u_pan_id, 1, &this->_u_pan_d[0]);
	} else {
		glUniform1f(this->_u_scale_id, this->_u_scale);
		glUniform2fv(this->_u_pan_id, 1, &this->_u_pan[0]);
	}

	/* Apply attributes. */
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	/* Draw triangle fan. */
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableVertexAttribArray(0); /* DO NOT REMOVE! */

	glfwSwapBuffers(this->_window);
}

void gl_controller::proc_movement() {
	if (this->keydown(GLFW_KEY_A)) {
		if (this->_use_double)
			this->_u_pan_d.x = _u_pan_d.x - 0.01 * _u_scale_d;
		else
			this->_u_pan.x = _u_pan.x - 0.01f * _u_scale;
	} else if (this->keydown(GLFW_KEY_D)) {
		if (this->_use_double)
			this->_u_pan_d.x = _u_pan_d.x + 0.01 * _u_scale_d;
		else
			this->_u_pan.x = _u_pan.x + 0.01f * _u_scale;
	}
	if (this->keydown(GLFW_KEY_W)) {
		if (this->_use_double)
			this->_u_pan_d.y = _u_pan_d.y + 0.01 * _u_scale_d;
		else
			this->_u_pan.y = _u_pan.y + 0.01f * _u_scale;
	} else if (this->keydown(GLFW_KEY_S)) {
		if (this->_use_double)
			this->_u_pan_d.y = _u_pan_d.y - 0.01 * _u_scale_d;
		else
			this->_u_pan.y = _u_pan.y - 0.01f * _u_scale;
	}
	if (this->keydown(GLFW_KEY_Z)) {
		if (this->_use_double)
			this->_u_scale_d = _u_scale_d - (0.01 * _u_scale_d);
		else
			this->_u_scale = _u_scale - (0.01f * _u_scale);
	} else if (this->keydown(GLFW_KEY_X)) {
		if (this->_use_double)
			this->_u_scale_d = _u_scale_d + (0.01 * _u_scale_d);
		else
			this->_u_scale = _u_scale + (0.01f * _u_scale);
	}
}

int gl_controller::poll() const {
	glfwPollEvents();
	int quit = glfwWindowShouldClose(this->_window);
	if (this->keydown(GLFW_KEY_Q))
		quit = 1;
	return quit;
}

bool gl_controller::keydown(int key) const noexcept {
	if (this->_keysdown.count(key) == 1) {
		return this->_keysdown.at(key);
	} else {
		return false;
	}
}
