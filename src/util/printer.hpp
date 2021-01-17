#ifndef UTIL_PRINTER_HPP
#define UTIL_PRINTER_HPP
#include <string>
#include <mutex>
#include <iostream>

namespace hwfractal {
	class printer {
	private:
		inline static std::mutex _cout_lock;
	public:
		inline static bool show_debug = true;
		inline static bool show_info = true;
		static void debug(const std::string &&string) {
			if (printer::show_debug) {
				printer::_cout_lock.lock();
				std::cout << "DEBUG: " << string << std::endl;
				printer::_cout_lock.unlock();
			}
		}
		static void info(const std::string &&string) {
			if (printer::show_info) {
				printer::_cout_lock.lock();
				std::cout << "INFO: " << string << std::endl;
				printer::_cout_lock.unlock();
			}
		}
	};
}

#endif
