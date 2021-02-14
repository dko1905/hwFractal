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

opencl = dependency('OpenCL')

executable(
	'hwfractal',
	[
		'src/main.c',
		'src/clw/clw.c'
	],
	dependencies: [
		opencl
	]
)
