# bgfx-qt5-win
Skeleton program to integrate Qt5 + bgfx on Windows platform

## How to build

### Build BGFX
You should clone bgfx git repository and its dependencies.
In some folder with writting permissions please run:
```
git clone https://github.com/bkaradzic/bgfx
git clone https://github.com/bkaradzic/bimg
git clone https://github.com/bkaradzic/bx
cd bgfx
..\bx\tools\bin\windows\genie --with-tools --with-dynamic-runtime --with-examples vs2019
```

If you have MSVC 2017 then use 'vs2017' keyword instead. It seems that genie supports other MSVC versions, but I did not try.
Open the generated bgfx\.build\projects\bgfx.sln in Visual Studio IDE and switch build type to 'Release', switch architecture to 'x64'.
Build it (F7 key usually).

### Build or download QT5 for your compiler and platform
You should obtain QT5 library which is built for your MSVC compiler and platform.

### Build BGFX_QT project
You must have CMake installed (https://cmake.org/).

Clone this repo (type in the previous cmd console):
```
cd ..
git clone https://github.com/PetrPPetrov/bgfx-qt5-win
cd bgfx-qt5-win
mkdir build
cd build
cmake-gui ..
```

Press 'Configure' button. CMake GUI will shows error that QT_ROOT is not specified:
```
Need to specify QT_ROOT
```

Using CMake GUI UI elements you need to specify path to your QT5 library. When press 'Configure' button again.
Then you need to specify BGFX_ROOT, BIMG_ROOT and BX_ROOT (where you cloned them previously).
Press 'Configure' button again. When 'Configuring done' you need to press 'Generate' button, then 'Open Project'.
The BGFX_QT project will be opened in VS IDE.
Please select Release (or RelWithDebInfo) configuration and build it (F7 key usually).
Select active project 'bgfx_qt' and start it (Ctrl-F5 key usually).
Enjoy cube from BGFX framework in QT5 window!
