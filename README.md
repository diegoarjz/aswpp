# aswpp

[AngelScript](https://www.angelcode.com/angelscript/) Wraper for C++.

Something to make it easier to register the application interface.

## Building

### Cloning aswpp

This is the easy step:

```sh
$ git clone https://github.com/diegoarjz/aswpp.git
```

From now on, `<aswpp_dir>` points to the directory created above.

### Installing dependencies

aswpp doesn't include the AngelScript sources but will download them as part of
its build process.

Building aswpp requires that you have [conan](https://conan.io/) and
[cmake](https://cmake.org/) installed. You can install the required versions with

```sh
$ python -m venv devenv
$ source ./devenv/bin/activate
$ pip install -r <aswpp_dir>/requirements.txt
```

Now that you have conan and cmake installed, you can get the remaining dependencies with conan.

```sh
$ conan install -of <build_dir> --build=missing <aswpp_dir>
```

`<build_dir>` is the directory where you want to build `aswpp`. If you haven't created a default conan profile, you may need to run `conan profile detect` before `conan install` above.

### Building aswpp

Building should also be done with `conan` as that will download the right AngelScript version and place it in the source tree.

```sh
$ conan build -of <build_dir> <aswpp_dir>
```
