#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include "exceptions/runtime_exception.hpp"

namespace hwfractal {
	class application {
	private:

	public:
		application() {
			throw runtime_exception("Hello world!");
		}
		void loop();
	};
}

#endif
