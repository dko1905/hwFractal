# HardWare accelerated Fractals
There are currently two branches, C and C++. Why, I wanted to compare what it
feels like writing the program in C and in C++. I am now pretty sure I won't
commit to the C++ branch, since it's so hard. I'm tearing my hair out, when
I'm writing C++.

## Compiling & running
### Dependencies
These dependencies don't include meson, which is our build system.
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

## Todo
- [X] Write README.md.
- [X] Basic project structure, including build system.
- [ ] Create GLFW window.
- [ ] ...

## License
Free as in Freedom.
[GPL-3.0-or-later](./LICENSE)
