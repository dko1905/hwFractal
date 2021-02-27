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

vulkanDep = dependency('vulkan')
glfwDep = dependency('glfw3', version: '>= 3')

executable(
	'hwfractal',
	[
		'src/main.c',
		'src/application.c',
	],
	dependencies: [
		vulkanDep,
		glfwDep
	],
)
