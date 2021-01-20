project(
	'hwfractal',
	'cpp',
	license: 'GPL-3.0-or-later',
	default_options: [
		'cpp_std=c++17',
		'warning_level=1',
		'werror=false'
	],
)

# Create opengl dep, and only add ld flags on windows.
opengl = declare_dependency(
	link_args: '-lopengl32'
)
if build_machine.system() != 'windows'
	opengl = declare_dependency()
endif

glfw = dependency('glfw3', version: '>= 3')
glew = dependency('glew')
glm = dependency('glm')
threads = dependency('threads')

executable(
	'hwfractal',
	[
		'src/main.cpp',
		'src/application.cpp',
		'src/exceptions/runtime_exception.cpp',
		'src/util/config.cpp',
		'src/util/printer.cpp',
		'src/interfaces/core_controller.cpp',
		'src/gl/gl_controller.cpp',
		'src/gl/shaders.cpp'
	],
	dependencies: [
		glfw, glew, glm, threads, opengl
	],
	include_directories: [
		'src'
	]
)
