#include <fstream>
#include <string>
#include <istream>
#include <map>

#include "config.hpp"

using namespace hwfractal;

config::config(const std::string &config_path) {
	std::ifstream in(config_path, std::ios::in);
	std::string line, key;
	
	while (std::getline(in, line)) {
		size_t position = line.find("=");
		
		key = line.substr(0, position);
		line.erase(0, position + std::string("=").length());

		this->_map[key] = line;
	}
}

config::config(const std::string &&config_path) {
	std::ifstream in(config_path, std::ios::in);
	std::string line, key;
	
	while (std::getline(in, line)) {
		size_t position = line.find("=");
		
		key = line.substr(0, position);
		line.erase(0, position + std::string("=").length());

		this->_map[key] = line;
	}
}

const std::string &config::get(std::string &key) const noexcept {
	return this->_map.at(key);
}

const std::string &config::get(std::string &&key) const noexcept {
	return this->_map.at(key);
}

void config::set(std::string &key, std::string &value) noexcept {
	this->_map[key] = value;
}

void config::set(std::string &&key, std::string &value) noexcept {
	this->_map[key] = value;
}
