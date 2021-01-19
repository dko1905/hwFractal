# HardWare accelerated Fractals (C++)
## Compiling & running
### Dependencies
These dependencies don't include meson, which is the build system.
| OS / Distro       | Package names                         |
|-------------------|---------------------------------------|
| Void Linux        | glfw-devel, glm, glew-devel           |
| Ubuntu (or based) | libglfw3-dev, libglew-dev, libglm-dev |

### Compiling
```sh
$ meson setup builddir
$ meson compile -C builddir
```

### Running
Run `builddir/hwfractal`

## Todo
- [X] Write README.md.
- [X] Basic project structure, including build system.
- [X] Create GLFW window.
- [X] Use shaders to render.
- [X] Fill window with TriangleFan.
- [X] Capture keys.
- [X] Movement, panning and zoom.
- [X] Actual mandelbrot shader.
- [ ] Max iteration uniform.
- [ ] Dynamic iteration, aka do frames with lower iteration and then higher.
- [ ] ...

## License
Free as in Freedom. [GPL-3.0-or-later](./LICENSE)
