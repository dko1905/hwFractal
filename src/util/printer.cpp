#include <iostream>

#include "printer.hpp"

using namespace hwfractal;

static bool _show_debug = true;
static bool _show_info = true;

bool &printer::show_debug() {
	return _show_debug;
}

bool &printer::show_info() {
	return _show_info;
}

void printer::debug(const std::string &&string) {
	if (_show_debug) {
		printer::_cout_lock.lock();
		std::cout << "DEBUG: " << string << std::endl;
		printer::_cout_lock.unlock();
	}
}

void printer::info(const std::string &&string) {
	if (_show_info) {
		printer::_cout_lock.lock();
		std::cout << "INFO: " << string << std::endl;
		printer::_cout_lock.unlock();
	}
}
