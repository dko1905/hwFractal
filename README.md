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
There should be a binary in `builddir/` called hwfractal. Just run it.

## License
Free as in Freedom. [GPL-3.0-or-later](./LICENSE)
