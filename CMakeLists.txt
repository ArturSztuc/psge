cmake_minimum_required(VERSION 3.16)

project(PintSizedProject LANGUAGES CXX)

# Global C++ setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Will create compile_commands.json for autocompleting
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# GLFW
#TODO: Will need vulkan, object loaders
find_package(glfw3 3.3)

if(glfw_FOUND)
    message("Found glfw")
else()
    message("Did not find GLFW. We will compile it instead.")
    set(glfw_DIR ${PROJECT_SOURCE_DIR}/Externals/GLFW)
    set(glfw_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation" FORCE)
    set(glfw_INSTALL OFF CACHE INTERNAL "Generate installation target" FORCE)
    add_subdirectory(${glfw_DIR})
endif(glfw_FOUND)

# The engine itself
add_subdirectory(Engine)

# Sandbox application
add_subdirectory(Game)

# Unit tests
#add_subdirectory(UnitTests)