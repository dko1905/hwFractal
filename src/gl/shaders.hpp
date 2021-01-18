#ifndef GL_SHADERS_HPP
#define GL_SHADERS_HPP
#include <sstream>
#include <string>
#include <thread>
#include <fstream>

#include <vector>
#include <iostream>

namespace hwfractal {
	namespace gl {
		class shaders {
		public:
			static GLuint compile(const std::string &vertex_path, const std::string &fragment_path);
		};
	}
}

#endif
