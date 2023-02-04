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
    set(glfw_DIR ${PROJECT_SOURCE_DIR}/Externals/glfw)
    set(glfw_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation" FORCE)
    set(glfw_INSTALL OFF CACHE INTERNAL "Generate installation target" FORCE)
    add_subdirectory(${glfw_DIR})
endif(glfw_FOUND)

find_package(Vulkan REQUIRED)
target_include_directories(glfw PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(glfw Vulkan::Vulkan)

find_package(nlohmann_json)
if(nlohmann_json_FOUND)
    message("Found nlohmann_json")
else()
    message("Did not find nlohmann_json. We will compile it instead.")
    set(nlohmann_json_DIR ${PROJECT_SOURCE_DIR}/Externals/json)
    set(JSON_Install OFF CACHE INTERNAL "")
    add_subdirectory(${nlohmann_json_DIR})
endif(nlohmann_json_FOUND)

# The engine itself
add_subdirectory(Engine)

# Sandbox application
add_subdirectory(Game)

# Unit tests
find_package(GTest)
if(GTest_FOUND)
    message("Found GTest")
else()
    message("Did not find GTest. We will compile it instead.")
    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG release-1.11.0
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif(GTest_FOUND)
add_subdirectory(UnitTests)