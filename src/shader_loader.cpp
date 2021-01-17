#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "exceptions/runtime_exception.hpp"

#include "shader_loader.hpp"

using namespace hwfractal;

static std::string err_str;

void shader_loader::init(const std::string &vertex_path, const std::string &fragment_path) {
	unsigned int vertexshader_id = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentshader_id = glCreateShader(GL_FRAGMENT_SHADER);
	
	std::thread vertex_thread ([&vertexshader_id, vertex_path](){
		/* Read shader from file. */
		std::string code;
		std::ifstream code_stream(vertex_path, std::ios::in);
		if (code_stream.is_open()) {
			std::stringstream sstr;
			sstr << code_stream.rdbuf();
			code = sstr.str();
			code_stream.close();
		} else {
			throw runtime_exception("Failed to open vertex shader.");
		}
		
		/* Compile shader. */
		char const * code_ptr = code.c_str();
		glShaderSource(vertexshader_id, 1, &code_ptr, NULL);
		glCompileShader(vertexshader_id);

		/* Check for errors. */
		int result = 0;
		int info_log_len = 0;
		glGetShaderiv(vertexshader_id, GL_COMPILE_STATUS, &result);
		if (info_log_len > 0) {
			std::vector<char> errormsg (info_log_len + 1);
			glGetShaderInfoLog(vertexshader_id, info_log_len, NULL, errormsg.data());
			err_str = "Errors checking vertex shader: ";
			err_str += std::string(errormsg.begin(), errormsg.end());
			throw runtime_exception(err_str.c_str());
		}
	});

	std::thread fragment_thread ([&fragmentshader_id, fragment_path](){
		/* Read shader from file. */
		std::string code;
		std::ifstream code_stream(fragment_path, std::ios::in);
		if (code_stream.is_open()) {
			std::stringstream sstr;
			sstr << code_stream.rdbuf();
			code = sstr.str();
			code_stream.close();
		} else {
			throw runtime_exception("Failed to open vertex shader.");
		}
		
		/* Compile shader. */
		char const * code_ptr = code.c_str();
		glShaderSource(fragmentshader_id, 1, &code_ptr, NULL);
		glCompileShader(fragmentshader_id);

		/* Check for errors. */
		int result = 0;
		int info_log_len = 0;
		glGetShaderiv(fragmentshader_id, GL_COMPILE_STATUS, &result);
		if (info_log_len > 0) {
			std::vector<char> errormsg (info_log_len + 1);
			glGetShaderInfoLog(fragmentshader_id, info_log_len, NULL, errormsg.data());
			err_str = "Errors checking vertex shader: ";
			err_str += std::string(errormsg.begin(), errormsg.end());
			throw runtime_exception(err_str.c_str());
		}
	});

	vertex_thread.join();
	fragment_thread.join();

	/* Link program. */
	unsigned int programid = glCreateProgram();
	glAttachShader(programid, vertexshader_id);
	glAttachShader(programid, fragmentshader_id);

	/* Check program. */
	int result = 0, info_log_len = 0;
	glGetProgramiv(programid, GL_LINK_STATUS, &result);
	glGetProgramiv(programid, GL_INFO_LOG_LENGTH, &info_log_len);
	if (info_log_len > 0) {
		std::vector<char> errormsg (info_log_len + 1);
		glGetShaderInfoLog(fragmentshader_id, info_log_len, NULL, errormsg.data());
		err_str = "Errors checking linking shaders: ";
		err_str += std::string(errormsg.begin(), errormsg.end());
		throw runtime_exception(err_str.c_str());
	}

	glDetachShader(programid, vertexshader_id);
	glDetachShader(programid, fragmentshader_id);

	glDeleteShader(vertexshader_id);
	glDeleteShader(fragmentshader_id);

	this->_programid = programid;
}
