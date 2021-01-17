#include "runtime_exception.hpp"

const char* hwfractal::runtime_exception::what() const noexcept {
	return this->_what;
}
