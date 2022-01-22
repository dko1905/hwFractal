# HardWare accelerated Fractals (C++)
![C++ CI](https://github.com/Dko1905/hwFractal/workflows/C++%20CI/badge.svg?branch=cpp)
## Compiling & running
On Linux you will need to recompile it, on Windows you can just extract and run the newest release.
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

Keymap:

| **Key** | **Action**           |
|---------|----------------------|
| W       | Up                   |
| A       | Left                 |
| S       | Down                 |
| D       | Right                |
| Z       | Zoom in              |
| X       | Zoom out             |
| C       | Increment C1 (Julia) |
| V       | Decrement C1 (Julia) |
| N       | Increment C2 (Julia) |
| M       | Decrement C2 (Julia) |

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
- [X] Julia Set
- [ ] Better FPS
- [ ] Smart config defaulting.
- [ ] Good key handling.
- [ ] Dynamic shader switching.
- [ ] ...

## License
Free as in Freedom. [GPL-3.0-or-later](./LICENSE)
