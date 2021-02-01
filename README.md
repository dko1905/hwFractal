# HardWare accelerated Fractals (C++)
![C++ CI](https://github.com/Dko1905/hwFractal/workflows/C++%20CI/badge.svg?branch=cpp)
## Compiling & running
On Linux you will need to recompile it, on Windows you can just extract and run the newst release.
The releases are 64-bit only. 

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
- [X] Max iteration uniform.
- [X] Dynamic iteration, aka do frames with lower iteration and then higher.
- [ ] Smart config defaulting.
- [ ] Good key handling.
- [ ] Dynamic shader switching.
- [ ] ...

## License
Free as in Freedom. [GPL-3.0-or-later](./LICENSE)
