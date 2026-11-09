# Programmation des Resaux Project

This project concerns a client server implementation for the [awale](https://en.wikipedia.org/wiki/Oware) game.

# Prerequisites

- a C compiler (clang or gcc)
- [meson](https://mesonbuild.com/) and [ninja](https://ninja-build.org/) installed and on `PATH`
- readline development library (`libreadline-dev` on Debian/Ubuntu)

# Compile and Run

```sh
meson setup build        # first time only
meson compile -C build
./build/awale_server     # terminal 1
./build/awale_client 127.0.0.1 8080   # terminal 2
```
