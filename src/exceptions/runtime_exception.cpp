#include "runtime_exception.hpp"

using namespace hwfractal;

const char* runtime_exception::what() const noexcept {
	return this->_what;
}
