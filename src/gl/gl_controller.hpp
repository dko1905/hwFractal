#ifndef GL_CONTROL_HPP
#define GL_CONTROL_HPP
#include <memory>

#include "gl.hpp"
#include "util/config.hpp"
#include "interfaces/core_controller.hpp"

namespace hwfractal {
	namespace gl {
		static const float vertex_buffer_data[] = {
			1.f, 1.f, 0.f,
			1.f, -1.f, 0.f,
			-1.f, -1.f, 0.f,
			-1.f, 1.f, 0.f
		};
		/* Control*/
		class gl_controller : public interfaces::core_controller {
		private:
			std::map<int, bool> _keysdown;
		
			/* Uniforms (all). */
			bool _use_double = _config->get("GL_USE_DOUBLE") == "YES" ? 1 : 0;
			bool _use_dynitr = _config->get("GL_USE_DYNITR") == "YES" ? 1 : 0;
			glm::vec2 _u_resolution {0.f};
			GLint _u_iter_max = _use_dynitr ? atoi(_config->get("GL_DYNITR_MIN").c_str()) : 2000;
			bool changed;

			GLuint _u_scale_id = 0;
			GLuint _u_pan_id = 0;
			GLuint _u_resolution_id = 0;
			GLuint _u_iter_max_id = 0;
			/* Uniforms (float). */
			float _u_scale = 0.005267f;
			glm::vec2 _u_pan {0.f};
			/* Uniforms (double). */
			double _u_scale_d = 0.005267;
			glm::dvec2 _u_pan_d {0.};

			/* OpenGL stuff. */
			GLFWwindow *_window = NULL;
			GLuint _vertex_array_id = 0;
			GLuint _vertexbuffer = 0;
			GLuint _program_id = 0;
		public:
			gl_controller(const std::shared_ptr<const config> &config);
			virtual ~gl_controller();
			virtual void proc_movement();
			virtual void render() const;
			virtual int poll() const;
			virtual bool keydown(int key) const noexcept;
			virtual double get_time() const noexcept;
		};
	}
}

#endif