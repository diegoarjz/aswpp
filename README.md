# ASWPP

[AngelScript](https://www.angelcode.com/angelscript/) Wraper for C++.

Something to make it easier to register the application interface.

## Building

ASWPP will download and build AngelScript as part of its build process.

```sh
$ mkdir build
$ conan install . -of build --build=missing
$ cmake --preset conan-release
$ cmake --build build
```
