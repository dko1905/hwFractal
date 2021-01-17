#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <memory>
#include <iostream>

extern "C" {
	#include <GL/glew.h>
	#include <GLFW/glfw3.h>
}
#include <glm/glm.hpp>

#include "exceptions/runtime_exception.hpp"
#include "shader_loader.hpp"
#include "util/config.hpp"
#include "gl/control.hpp"

namespace hwfractal {
	class application {
	private:
		std::shared_ptr<config> _config;
		std::unique_ptr<gl::control> _gl_control;
		
	public:
		application(const std::shared_ptr<config> &config);
	};
}

#endif
