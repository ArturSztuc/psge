# Finding or findig and compiling the dependencies

# Vulkan is required
find_package(Vulkan REQUIRED)

# Find GLFW, compile from Externals if not installed
find_package(glfw3 3.3)
if(glfw3_FOUND)
    message("Found glfw")
else()
    message("Did not find GLFW. We will compile it instead.")
    # Adds the local GLFW directory
    set(GLFW_DIR ${PROJECT_SOURCE_DIR}/Externals/glfw)
    # Disables all the extras....
    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation" FORCE)
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target" FORCE)
    # Adding the glfw library and linking Vulkan to it
    add_subdirectory(${GLFW_DIR})
    target_include_directories(glfw PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(glfw Vulkan::Vulkan)
endif(glfw3_FOUND)

# Find nlohmann/json, compile from Externals if not installed
find_package(nlohmann_json)
if(nlohmann_json_FOUND)
    message("Found nlohmann_json")
else()
    message("Did not find nlohmann_json. We will compile it instead.")
    # Adds json directory
    set(nlohmann_json_DIR ${PROJECT_SOURCE_DIR}/Externals/json)
    # Not actually installing it, we only need the headers
    set(JSON_Install OFF CACHE INTERNAL "")
    add_subdirectory(${nlohmann_json_DIR})
endif(nlohmann_json_FOUND)

# Find nlohmann/json, download and link if not installed
find_package(GTest)
if(GTest_FOUND)
    message("Found GTest")
else()
    message("Did not find GTest. We will compile it instead.")
    # We fetch google test from github
    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG release-1.11.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    # Making the GTest::gtest_main available
    FetchContent_MakeAvailable(googletest)
endif(GTest_FOUND)