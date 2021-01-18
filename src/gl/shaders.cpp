#include <sstream>
#include <fstream>
#include <vector>

#include "exceptions/runtime_exception.hpp"
#include "util/printer.hpp"

#include "shaders.hpp"

using namespace hwfractal;

static std::string err_str;

GLuint gl::shaders::compile(const std::string &vertex_path, const std::string &fragment_path) {
	GLuint vertexshader_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentshader_id = glCreateShader(GL_FRAGMENT_SHADER);
	
	{
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
		
		printer::debug("Read fragment shader.");
		/* Compile shader. */
		char const * code_ptr = code.c_str();
		glShaderSource(fragmentshader_id, 1, &code_ptr, NULL);
		glCompileShader(fragmentshader_id);

		/* Check for errors. */
		int result = 0;
		int info_log_len = 0;
		glGetShaderiv(fragmentshader_id, GL_COMPILE_STATUS, &result);
		glGetShaderiv(fragmentshader_id, GL_INFO_LOG_LENGTH, &info_log_len);
		if (info_log_len > 0) {
			std::vector<char> errormsg (info_log_len + 1);
			glGetShaderInfoLog(fragmentshader_id, info_log_len, NULL, errormsg.data());
			err_str = "Errors checking vertex shader: ";
			err_str += std::string(errormsg.begin(), errormsg.end());
			throw runtime_exception(err_str.c_str());
		}
		printer::debug("Compiled fragmet shader without errors.");
	}

	{
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
		
		printer::debug("Read vertex shader.");
		/* Compile shader. */
		char const * code_ptr = code.c_str();
		glShaderSource(vertexshader_id, 1, &code_ptr, NULL);
		glCompileShader(vertexshader_id);

		/* Check for errors. */
		GLint result = GL_FALSE;
		int info_log_len = 0;
		glGetShaderiv(vertexshader_id, GL_COMPILE_STATUS, &result);
		glGetShaderiv(vertexshader_id, GL_INFO_LOG_LENGTH, &info_log_len);
		if (info_log_len > 0) {
			std::vector<char> errormsg (info_log_len + 1);
			glGetShaderInfoLog(vertexshader_id, info_log_len, NULL, errormsg.data());
			err_str = "Errors checking vertex shader: ";
			err_str += std::string(errormsg.begin(), errormsg.end());
			throw runtime_exception(err_str.c_str());
		}
		printer::debug("Compiled vertex shader without errors.");
	}

	/* Link program. */
	unsigned int programid = glCreateProgram();
	glAttachShader(programid, vertexshader_id);
	glAttachShader(programid, fragmentshader_id);
	glLinkProgram(programid);

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
	printer::debug("Linked shaders without errors.");

	glDetachShader(programid, vertexshader_id);
	glDetachShader(programid, fragmentshader_id);

	glDeleteShader(vertexshader_id);
	glDeleteShader(fragmentshader_id);

	return programid;
}
