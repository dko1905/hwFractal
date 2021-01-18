#ifndef INTERFACES_CORE_CONTROLLER_HPP
#define INTERFACES_CORE_CONTROLLER_HPP
#include <memory>

#include "util/config.hpp"

namespace hwfractal {
	namespace interfaces {
		class core_controller {
		protected:
			std::shared_ptr<const config> _config;
		public:
			core_controller(const std::shared_ptr<const config> &config);
			virtual ~core_controller() = default;
			/* Render to window. */
			virtual void render() const = 0;
			/* Polls for events, returns 1 if program should exit, else 0. */
			virtual int poll() const = 0;
		};
	}
}

#endif
