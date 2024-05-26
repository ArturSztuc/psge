Project(RendererTestPlugin)
set(NAME RendererTestPlugin)

include(${CMAKE_SOURCE_DIR}/Externals/CMakeLists.txt)

find_package(PintSizedGameEngine) # REQUIRED PATHS ${CMAKE_PREFIX_PATH} NO_DEFAULT_PATH)
if(PintSizedGameEngine_FOUND)
  message(STATUS "Found Pint Sized Game Engine!")
else()
  #message(FATAL_ERROR "Not found Pint Sized Game Engine...")
endif(PintSizedGameEngine_FOUND)

include_directories(${PintSizedGameEngine_INCLUDE_DIRS})

# Get all the game sources
file(GLOB_RECURSE SOURCESS ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERSS ${PROJECT_SOURCE_DIR}/*.hpp)

# Add the game executable
add_library(${PROJECT_NAME} SHARED ${SOURCESS})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(${PROJECT_NAME} PRIVATE ${HEADERSS})

## Link the engine library
target_link_libraries(${PROJECT_NAME} 
                #PintSizedGameEngine
                ${PintSizedGameEngine_LIBRARY_DIRS}/lib${PintSizedGameEngine_LIBRARIES}.so
                glfw ${GLFW_LIBRARIES}
                nlohmann_json::nlohmann_json
                Boost::filesystem)

target_include_directories(${PROJECT_NAME} PRIVATE ${PintSizedGameEngine_INCLUDE_DIRS})

set(ASSETS_DIR "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/assets/")
add_compile_definitions(ASSETS_DIR="${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/assets/")
add_definitions(-DASSETS_DIR="${ASSETS_DIR}")
