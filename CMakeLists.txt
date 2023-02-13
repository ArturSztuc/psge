cmake_minimum_required(VERSION 3.16)

project(PintSizedProject LANGUAGES CXX)

# Global C++ setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GRAPHVIZ_CUSTOM_TARGETS TRUE)

# Will create compile_commands.json for autocompleting
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# The engine itself
add_subdirectory(Engine)

# Sandbox application
add_subdirectory(Game)

# Unit tests
add_subdirectory(UnitTests)

include(Externals/dependency-graph.cmake)
gen_dep_graph(png)