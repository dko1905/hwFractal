#include "application.hpp"

#include "gl/gl_controller.hpp"
#include "util/printer.hpp"

using namespace hwfractal;

application::application(const std::shared_ptr<const config> &config) {
	this->_config = config;
	this->_core_controller = std::make_unique<gl::gl_controller>(config);

	int exit = 0;
	do {
		this->_core_controller->render();
		exit = this->_core_controller->poll();
	} while (
		exit == 0
	);
}
