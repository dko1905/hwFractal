#ifndef SHADER_UTIL_HPP
#define SHADER_UTIL_HPP
#include <string>

extern "C" {
	#include <GL/glew.h>
	#include <GLFW/glfw3.h>
}
#include <glm/glm.hpp>

#include "exceptions/runtime_exception.hpp"

namespace hwfractal {
	class shader_loader {
	private:
		bool _initialized = false;
		/* OpenGL stuff. */
		int _programid = 0; /* This will be set after `init()`. */
		
	public:
		shader_loader() {
			if (this->_initialized) {
				throw runtime_exception(
					"application contructor called, when already initialized."
				);
			} else {
				this->_initialized = false;
			}
		}
		~shader_loader() {
			if (this->_initialized) {
				
				this->_initialized = false;
			}
		}
		/**
		 * @brief Initialize OpenGL.
		 */
		void init(const std::string &vertex_path, const std::string &fragment_path);
		/**
		 * @brief Get programid from shaders.
		 */
		const int &programid() const {
			return this->_programid;
		}
	};
}

#endif
