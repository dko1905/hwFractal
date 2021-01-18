#include <memory>
#include <iostream>

#include "exceptions/runtime_exception.hpp"
#include "interfaces/core_controller.hpp"
#include "util/printer.hpp"
#include "shaders.hpp"

#include "gl_controller.hpp"

using namespace hwfractal;
using namespace gl;

static std::map<int, bool> *keysdown = NULL;

gl_controller::gl_controller(const std::shared_ptr<const config> &config) : core_controller(config) {
	this->_config = config;

	glewExperimental = true; /* Needed for core profile. */
	if (!glfwInit()) {
		throw runtime_exception("Failed to initialize GLFW");
	}
	/* Request OpenGL 4.1. */
	glfwWindowHint(GLFW_SAMPLES, atoi(
		this->_config->get("GL_GLFW_SAMPLES").c_str()
	));
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); /* Make MacOS happy. */
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	/* Open window. */
	this->_window = glfwCreateWindow( 600, 400, "hwfractal", NULL, NULL);
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
		this->_config->get("GL_SHADER_FPATH")
	);
	/* Setup vertexbuffer. */
	glGenBuffers(1, &this->_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gl::vertex_buffer_data), gl::vertex_buffer_data, GL_STATIC_DRAW);
	/* Setup callbacks. */
	GLFWframebuffersizefun resisze_lamda = [](GLFWwindow* _w, int width, int height) {
		glViewport(0, 0, width, height);
	};
	GLFWkeyfun key_lamda = [](GLFWwindow* _w, int key, int _scancode, int action, int _mods) {
		if(action == GLFW_PRESS || action == GLFW_RELEASE){
			if(action == GLFW_PRESS){
				(*keysdown)[key] = true;
			} else{
				(*keysdown)[key] = false;
			}
		}
	};
	keysdown = &this->_keysdown;
	glfwSetFramebufferSizeCallback(this->_window, resisze_lamda);
	glfwSetKeyCallback(this->_window, key_lamda);
}

gl_controller::~gl_controller() {
	/* Cleanup any resources used by OpenGL. */
	if (this->_window != NULL) {
		/* Remove callbacks. */
		glfwSetKeyCallback(this->_window, NULL);
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

int gl_controller::poll() const {
	glfwPollEvents();
	return glfwWindowShouldClose(this->_window);
}

bool gl_controller::keydown(int key) const noexcept {
	if (this->_keysdown.count(key) == 1) {
		return this->_keysdown.at(key);
	} else {
		return false;
	}
}
