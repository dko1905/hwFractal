#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <memory>
#include <iostream>

#include "exceptions/runtime_exception.hpp"
#include "util/config.hpp"
#include "gl/gl_controller.hpp"
#include "interfaces/core_controller.hpp"

namespace hwfractal {
	class application {
	private:
		std::shared_ptr<const config> _config;
		std::unique_ptr<interfaces::core_controller> _core_controller;
		
	public:
		application(const std::shared_ptr<const config> &config);
	};
}

#endif
