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
		'src/gl/control.cpp',
		'src/gl/shaders.cpp'
	],
	dependencies: [
		glfw, glew, glm, threads
	],
	include_directories: [
		'src'
	]
)
