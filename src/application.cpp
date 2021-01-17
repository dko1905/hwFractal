#include <iostream>

#include "application.hpp"

using namespace hwfractal;

void application::init() {
	glewExperimental = GL_TRUE;
	if (!glfwInit()) {
		throw runtime_exception("Failed to initialize GLFW.");
	}
	glfwWindowHint(GLFW_SAMPLES, 4); /* 4x antialiasing. */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); /* We want at least 4.0. */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); /* Make MacOS happy.*/
	/* We don't want old OpenGL. */
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create window. */
	auto window = glfwCreateWindow(640, 480, "hwfractal", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw runtime_exception("Failed to create window.");
	}
	this->_window = window;

	/* Initialize GLEW. */
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		glfwTerminate(); /* Should maybe be there? */
		throw runtime_exception("Failed to initialize GLEW.");
	}

	/* VAO. */
	glGenVertexArrays(1, &this->_vertex_array_id);
	glBindVertexArray(this->_vertex_array_id);

	/* Setup vertexbuffer. */
	glGenBuffers(1, &this->_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hwfractal::vertexbuffer_triangle_buffer), hwfractal::vertexbuffer_triangle_buffer, GL_STATIC_DRAW);

	/* Catch "all" keys. */
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	sl.init("resources/test.vs", "resources/test.fs");

	this->_initialized = true;
}

void application::loop() {
	do {
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(sl.programid());
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
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);

		glfwSwapBuffers(this->_window);
		glfwPollEvents();
	} while (
		glfwGetKey(this->_window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		glfwWindowShouldClose(this->_window) == 0
	);
}
