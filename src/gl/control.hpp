#ifndef GL_CONTROL_HPP
#define GL_CONTROL_HPP
#include <memory>

#include "gl.hpp"
#include "util/config.hpp"

namespace hwfractal {
	namespace gl {
		static const float vertex_buffer_data[] = {
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			0.0f,  1.0f, 0.0f,
		};
		/**
		 * @brief `control` class controls a GLFW/GLEW backend.
		 */
		class control {
		private:
			std::shared_ptr<const config> _config;
			GLFWwindow *_window = NULL;
			GLuint _vertex_array_id = 0;
			GLuint _vertexbuffer = 0;
			GLuint _program_id = 0;
			
		public:
			control(const std::shared_ptr<const config> &config);
			~control();
			void render() const;
			int poll() const;
		};
	}
}

#endif