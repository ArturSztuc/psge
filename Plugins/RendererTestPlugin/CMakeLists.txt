Project(RendererTestPlugin)
set(NAME RendererTestPlugin)

include(${CMAKE_SOURCE_DIR}/Externals/CMakeLists.txt)

find_package(PintSizedGameEngine)
if(PintSizedGameEngine_FOUND)
  message("Found Pint Sized Game Engine!")
endif(PintSizedGameEngine_FOUND)

# Get all the game sources
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/*.hpp)

# Add the game executable
add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(${PROJECT_NAME} PRIVATE ${HEADERS})

## Set the name of the output shared library (adjust as needed).
#set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
#
## Link the engine library
target_link_libraries(${PROJECT_NAME} PintSizedGameEngine glfw ${GLFW_LIBRARIES} nlohmann_json::nlohmann_json Boost::filesystem)
target_include_directories(${PROJECT_NAME} PRIVATE ${PintSizedGameEngine_INCLUDE_DIRS})
#
#set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Plugins)
#
set(ASSETS_DIR "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/assets/")
add_compile_definitions(ASSETS_DIR="${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/assets/")
add_definitions(-DASSETS_DIR="${ASSETS_DIR}")
