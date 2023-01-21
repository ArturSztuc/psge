# PSGE

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
│   └── Graphics
├── Game
│   ├── Assets
│   └── Sources
└── UnitTests
    └── Core
```

`Engine` is compiled as a library that contains an entry point `main`.\
`Game` is a sandbox that needs to have an object derived from the `Engine`'s
`Application` and have a function `Application* CreateApplication()` that links
to the entry point.\
The entry point is inside of the `Engine/Application` folder.\
`UnitTests` contains unit tests written with a help of GoogleTest framework.\
All the objects inside of the `Engine` should to have their own unit tests
there.

## Building
```
cd psge
mkdir build
cd build
cmake ..
make
```

## Running
From now from `build` folder.
In the future the build will be with an `install` option that will distribute
the libraries/assets/executables in the appropriate system folders.

### Unit tests
```
cd UnitTests
ctest --verbose
```

### Sandbox
```
cd Game
./Sandbox
```
