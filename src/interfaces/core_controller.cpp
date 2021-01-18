#include "core_controller.hpp"

using namespace hwfractal::interfaces;

core_controller::core_controller(const std::shared_ptr<const config> &config) {
	this->_config = config;
}
