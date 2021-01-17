project(
	'hwfractal',
	'c',
	license: 'GPL-3.0-or-later',
	default_options: [
		'c_std=c99',
		'warning_level=1',
		'werror=false'
	],
)

glfw = dependency('glfw3', version: '>= 3')
glew = dependency('glew')
glm = dependency('glm')

executable(
	'hwfractal',
	[
		'src/main.c',
	],
	dependencies: [
		glfw, glew, glm
	]
)
