#ifndef EXCEPTIONS_RUNTIME_EXCEPTION_HPP
#define EXCEPTIONS_RUNTIME_EXCEPTION_HPP
#include <exception>

namespace hwfractal {
	class runtime_exception: public std::exception {
	private:
		const char *_what;
	public:
		runtime_exception(const char *message) {
			this->_what = message;
		}
		virtual const char* what() const noexcept;
	};
}

#endif
