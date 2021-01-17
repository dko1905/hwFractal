#ifndef GL_CONTROL_HPP
#define GL_CONTROL_HPP
#include <memory>

#include "../util/config.hpp"

namespace hwfractal {
	namespace gl {
		/**
		 * @brief `control` class controls a GLFW/GLEW backend.
		 */
		class control {
		private:
			std::shared_ptr<config> _config;
		public:
			control(const std::shared_ptr<config> &config) {
				this->_config = config;
			}
		};
	}
}

#endif