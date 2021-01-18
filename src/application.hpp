#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <memory>
#include <iostream>

#include "exceptions/runtime_exception.hpp"
#include "util/config.hpp"
#include "gl/control.hpp"

namespace hwfractal {
	class application {
	private:
		std::shared_ptr<const config> _config;
		std::unique_ptr<gl::control> _gl_control;
		
	public:
		application(const std::shared_ptr<const config> &config);
	};
}

#endif
