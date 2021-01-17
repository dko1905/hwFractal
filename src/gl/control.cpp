#include "control.hpp"
#include <memory>

extern "C" {
	#include <GL/glew.h>
	#include <GLFW/glfw3.h>
}
#include <glm/glm.hpp>
#include <cstdlib>

#include "../exceptions/runtime_exception.hpp"
#include "../util/printer.hpp"
#include "../keybind.hpp"
#include "shaders.hpp"

using namespace hwfractal::gl;

static void *global_map;

control::control(const std::shared_ptr<config> &config) {
	this->_config = config;

	glewExperimental = GL_TRUE;
	if (!glfwInit()) {
		throw runtime_exception("Failed to initialize GLFW.");
	}
	glfwWindowHint(GLFW_SAMPLES, std::atoi(
		(config->get("GL_GLFW_SAMPLES") != "" ? config->get("GL_GLFW_SAMPLES").c_str() : "4")
	));
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); /* Minimum 4.0 */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); /* Make MacOS happy.*/
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create window. */
	auto window = glfwCreateWindow(640, 480, "hwfractal", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw runtime_exception("Failed to create window.");
	}
	this->_window = window;
	
	/* Setup GLEW. */
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		throw runtime_exception("Failed to initialize GLEW.");
	}

	/* VAO */
	glGenVertexArrays(1, &this->_vertex_array_id);
	glBindVertexArray(this->_vertex_array_id);
	
	/* Setup vertexbuffer. */
	glGenBuffers(1, &this->_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hwfractal::gl::vertex_buffer_data), hwfractal::gl::vertex_buffer_data, GL_STATIC_DRAW);

	/* Catch special keys, e.g. ESC. */
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	/* Setup key callback. */
	global_map = &this->_keysdown;
	auto key_lambda = [](GLFWwindow* _w, int key, int _scancode, int action, int _mods){
		if(action == GLFW_PRESS || action == GLFW_RELEASE){
			auto keysdown = (std::map<keybind, bool> *)global_map;
			if(action == GLFW_PRESS){
				if (key == GLFW_KEY_Q) {
					keysdown->at(keybind::QUIT) = true;
				}
			} else{
				if (key == GLFW_KEY_Q) {
					keysdown->at(keybind::QUIT) = false;
				}
			}
		}
	};
	glfwSetKeyCallback(window, key_lambda);

	/* Load shaders. */
	this->_program_id = shaders::compile(config->get("GL_SHADER_VPATH"), config->get("GL_SHADER_FPATH"));

	/* Setup keymap to zero. */
	this->_keysdown[keybind::QUIT] = false;
}

control::~control() {
	glfwTerminate();
}

void control::render() const {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(this->_program_id);

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
	// Draw the triangle!
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);

	/* Swap buffers. */
	glfwSwapBuffers(this->_window);
}

void control::poll() const {
	glfwPollEvents();
}

const bool &control::keydown(const keybind &key) const {
	return this->_keysdown.at(key);
}
