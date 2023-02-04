# PSGE : Pint-Sized Game Engine

The current folder structure: 

```
psge
├── Engine
│   ├── Application
│   ├── Core
│   │   ├── Config
│   │   ├── DataStructures
│   │   ├── EntityComponentSystem
│   │   ├── Event
│   │   ├── Inputs
│   │   ├── Logging
│   │   ├── Memory
│   │   ├── String
│   │   ├── Timing
│   │   └── Window
│   ├── Graphics
│   └── Platform
├── Exterals
│   ├── glfw
│   └── json
├── Game
│   └── Sources
└── UnitTests
    └── Core
```

`Engine` is compiled as a library that contains an entry point `main`.\
`Externals` contains all the external dependencies (other than `Vulkan`).\
`Game` is a sandbox that needs to have an object derived from the `Engine`'s
`Application` and have a function `Application* CreateApplication()` that links
to the entry point.\
The entry point is inside of the `Engine/Application` folder.\
`UnitTests` contains unit tests written with a help of GoogleTest framework.\
All the objects inside of the `Engine` should to have their own unit tests
there.

## Building
The build should work under both Linux and Windows. It should be possible to build with VSCode, provided you have the correct CMake generators and compiler linked. For a simple build:
```
cd psge
mkdir build
cd build
cmake ..
make
```

### Dependencies
`Vulkan` needs to be installed to run the engine. This project searches for all the other dependencies and either links them if already installed, installs them from the `Externals` folder, or downloads and installs (e.g. `GTest`).

The required dependencies:
* cmake >= 3.16
* Vulkan

Dependencies that can be installed, but otherwise the project installs them:
* glfw3 == 3.3
* nlohmann/json.hpp
* GTest


This will only increase as we add glm, shaders.

## Running
From now from `build` folder.
In the future the build will be with an `install` option that will distribute
the libraries/assets/executables in the appropriate system folders.

### Unit tests
```
cd build/UnitTests
ctest --verbose
```

### Sandbox
```
cd build/Game
./Sandbox
```

