#ifndef INTERFACES_CORE_CONTROLLER_HPP
#define INTERFACES_CORE_CONTROLLER_HPP
#include <memory>

#include "util/config.hpp"

namespace hwfractal::interfaces {
	class core_controller {
	protected:
		std::shared_ptr<const config> _config;
	public:
		core_controller(const std::shared_ptr<const config> &config);
		virtual ~core_controller() = default;
		/* Process keypresses into movement. */
		virtual void proc_movement() = 0;
		/* Render to window. */
		virtual void render() const = 0;
		/* Polls for events, returns 1 if program should exit, else 0. */
		virtual int poll() const = 0;
		/* Check if key is down. */
		virtual bool keydown(int key) const noexcept = 0;
		/* Get time since last frame. */
		virtual double get_time() const noexcept = 0;
	};
}

#endif
