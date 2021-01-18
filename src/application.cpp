#include "application.hpp"

#include "util/printer.hpp"

using namespace hwfractal;

application::application(const std::shared_ptr<config> &config) {
	this->_config = config;
	this->_gl_control = std::move(std::make_unique<gl::control>(config));

	this->_gl_control->render();
	/*
	do {
		
		pr = this->_gl_control->poll();
	} while (
		this->_gl_control->keydown(keybind::QUIT) != true && pr == 0
	);*/
}
