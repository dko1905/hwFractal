#ifndef GL_CONTROL_HPP
#define GL_CONTROL_HPP
#include <memory>

#include "gl.hpp"
#include "util/config.hpp"
#include "interfaces/core_controller.hpp"

namespace hwfractal {
	namespace gl {
		static const float vertex_buffer_data[] = {
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			0.0f,  1.0f, 0.0f,
		};
		/* Control*/
		class gl_controller : public interfaces::core_controller {
		private:
			GLFWwindow *_window = NULL;
			GLuint _vertex_array_id = 0;
			GLuint _vertexbuffer = 0;
			GLuint _program_id = 0;
		public:
			gl_controller(const std::shared_ptr<const config> &config);
			virtual ~gl_controller();
			virtual void render() const;
			virtual int poll() const;
		};
	}
}

#endif