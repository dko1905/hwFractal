#include "gl/gl.hpp"

#include "gl/gl_controller.hpp"
#include "util/printer.hpp"

#include "application.hpp"

using namespace hwfractal;

application::application(const std::shared_ptr<const config> &config) {
	this->_config = config;
	this->_core_controller = std::make_unique<gl::gl_controller>(config);

	int exit = 0;
	std::size_t frames = 0;
	double lastTime = this->_core_controller->get_time();
	do {
		double currentTime = this->_core_controller->get_time();
		frames++;
		/* Print fps once a second. */
		if (currentTime - lastTime >= 0.25) {
			printer::info("MS/FRAME: " + std::to_string(250.0/double(frames)) + " ms FPS: " + std::to_string(double(frames)/0.25));
			frames = 0;
			lastTime += 0.25;
		}

		this->_core_controller->proc_movement();
		this->_core_controller->render();
		exit = this->_core_controller->poll();
	} while (
		this->_core_controller->keydown(GLFW_KEY_Q) != true &&
		exit == 0
	);
}
