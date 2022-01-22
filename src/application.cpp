#include "gl/gl.hpp"

#include "gl/gl_controller.hpp"
#include "util/printer.hpp"

#include "application.hpp"

#include <chrono>

using namespace hwfractal;

application::application(const std::shared_ptr<const config> &config) {
	this->_config = config;
	this->_core_controller = std::make_unique<gl::gl_controller>(config);

	int exit = 0;
	auto render_begin = std::chrono::high_resolution_clock::now();
	auto render_end = std::chrono::high_resolution_clock::now();
	auto last_print = std::chrono::high_resolution_clock::now();
	do {
		auto now = std::chrono::high_resolution_clock::now();
		auto dif = now - last_print;
		if (dif > std::chrono::milliseconds(25)) {
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(render_end - render_begin);
			auto fps = 1000.0 / ms.count();
			printer::info("MS/FRAME: " + std::to_string(ms.count()) + " " +
			              "FPS: " + std::to_string(fps));
			last_print = std::chrono::high_resolution_clock::now();
		}

		render_begin = std::chrono::high_resolution_clock::now();
		this->_core_controller->proc_movement();
		this->_core_controller->render();
		render_end = std::chrono::high_resolution_clock::now();
		exit = this->_core_controller->poll();
	} while (
		this->_core_controller->keydown(GLFW_KEY_Q) != true &&
		exit == 0
	);
}
