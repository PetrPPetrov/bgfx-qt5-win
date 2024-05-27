# bgfx-qt
Skeleton program to integrate Qt and bgfx

## How to build

### Build bgfx
You should clone bgfx git repository and its dependencies.

```
git submodule update --init --recursive
```

### Build or download Qt6 for your compiler and platform
You should obtain Qt6 library which is built for your compiler and platform.

### Build bgfx-qt project
You must have CMake installed (https://cmake.org/).

Clone this repo (type in the previous cmd console):
```
git clone https://github.com/theo-dep/bgfx-qt
cd bgfx-qt
cmake -S. -Bbuild # $CMakeOptions
cmake --build build --config Release --target all -j 8
```

Enjoy cube from bgfx framework in Qt window!
