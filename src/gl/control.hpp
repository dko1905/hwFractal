#ifndef GL_CONTROL_HPP
#define GL_CONTROL_HPP
#include <memory>

extern "C" {
	#include <GL/glew.h>
	#include <GLFW/glfw3.h>
}

#include "../util/config.hpp"
#include "../keybind.hpp"

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
			std::shared_ptr<config> _config;
			GLFWwindow *_window;
			unsigned int _vertex_array_id;
			unsigned int _vertexbuffer;
			unsigned int _program_id;

			std::map<keybind, bool> _keysdown;
		public:
			control(const std::shared_ptr<config> &config);
			~control();
			void render() const;
			void poll() const;
			const bool &keydown(const keybind &key) const;
		};
	}
}

#endif