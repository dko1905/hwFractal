#ifndef UTIL_CONFIG_HPP
#define UTIL_CONFIG_HPP
#include <string>
#include <map>

namespace hwfractal {
	class config {
	private:		
		std::map<std::string, std::string> _map;

		const std::string empty = "";
	public:
		config(const std::string &config_path);
		config(const std::string &&config_path);

		const std::string &get(std::string &key) const noexcept;
		const std::string &get(std::string &&key) const noexcept;

		void set(std::string &key, std::string &value) noexcept;
		void set(std::string &&key, std::string &value) noexcept;
	};
}

#endif
