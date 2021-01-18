#ifndef UTIL_PRINTER_HPP
#define UTIL_PRINTER_HPP
#include <string>
#include <mutex>

namespace hwfractal {
	class printer {
	private:
		inline static std::mutex _cout_lock;
	public:
		static bool &show_debug();
		static bool &show_info();
		static void debug(const std::string &&string);
		static void info(const std::string &&string);
	};
}

#endif
