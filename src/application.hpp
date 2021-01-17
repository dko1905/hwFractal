#ifndef APPLICATION_HPP
#define APPLICATION_HPP

extern "C" {
	#include <GL/glew.h>
	#include <GLFW/glfw3.h>
}
#include <glm/glm.hpp>

#include "exceptions/runtime_exception.hpp"
#include "shader_loader.hpp"

namespace hwfractal {
	static const float vertexbuffer_triangle_buffer[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};

	class application {
	private:
		bool _initialized = false;

		/* OpenGL stuff. */
		GLFWwindow *_window = NULL;
		unsigned int _vertex_array_id = 0;
		unsigned int _vertexbuffer = 0;

		shader_loader sl;

	public:
		application() {
			if (_initialized) {
				throw runtime_exception(
					"application contructor called, when already initialized."
				);
			} else {
				_initialized = false;
			}
		}
		~application() {
			if (_initialized) {
				glfwDestroyWindow(this->_window);
				this->_window = NULL;
				this->_initialized = false;
			}
		}
		/* Disable copy-contructor. */
		application(const application &) = delete;
		/**
		 * @brief Initialize OpenGL. 
		 */
		void init();
		/**
		 * @brief This is the main gameloop.
		 */
		void loop();
	};
}

#endif
