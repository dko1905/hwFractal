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

sdl2Dep = dependency('sdl2')
# tallocDep = dependency('talloc')
# glfwDep = dependency('glfw3', version: '>= 3')
# vulkanDep = dependency('vulkan')

executable(
	'hwfractal',
	[
		'src/main.c',
		'src/application.c',
		'src/lib/log.c',
		'src/lib/talloc/talloc.c'
	],
	dependencies: [
		sdl2Dep,
	],
	c_args: ['-DLOG_USE_COLOR']
)
