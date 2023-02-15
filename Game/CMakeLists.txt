project(Sandbox)
set(NAME Sandbox)

include(${CMAKE_SOURCE_DIR}/Externals/CMakeLists.txt)

# Get all the game sources
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/Sources/*.cpp)

find_package(PintSizedGameEngine)
if(PintSizedGameEngine_FOUND)
  message("Found Pint Sized Game Engine!")
endif(PintSizedGameEngine_FOUND)

# Add the game executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the engine library
target_link_libraries(${PROJECT_NAME} PintSizedGameEngine glfw ${GLFW_LIBRARIES} nlohmann_json::nlohmann_json Boost::filesystem)
target_include_directories(${PROJECT_NAME} PRIVATE ${PintSizedGameEngine_INCLUDE_DIRS})
