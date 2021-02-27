# HardWare accelerated Fractals (C-advanced)
![C-advanced CI](https://github.com/Dko1905/hwFractal/workflows/C-advanced%20CI/badge.svg?branch=c-advanced)
## Compiling & running
### Dependencies
These dependencies don't include meson, which is the build system.
| OS / Distro       | Package names                |
|-------------------|------------------------------|
| Void Linux        | opencl2-headers              |
| Ubuntu (or based) | opencl-c-headers, opencl-dev |

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
- [ ] Use shaders to render.
- [ ] Fill window with TriangleFan.
- [ ] Capture keys.
- [ ] Movement, panning and zoom.
- [ ] Actual mandelbrot shader.
- [ ] Max iteration uniform.
- [ ] Dynamic iteration, aka do frames with lower iteration and then higher.
- [ ] Smart config defaulting.
- [ ] Good key handling.
- [ ] Dynamic shader switching.
- [ ] ...

## License
Free as in Freedom. [GPL-3.0-or-later](./LICENSE)
