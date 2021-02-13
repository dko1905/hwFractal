#ifndef RUNTIME_CONFIG
#define RUNTIME_CONFIG

struct runtime_config {
	unsigned int glfw_version_minor;
	unsigned int glfw_version_major;
	const char *vertex_path;
	const char *fragment_path;
	const char *window_name;
	unsigned int width;
	unsigned int height;
};

#endif
