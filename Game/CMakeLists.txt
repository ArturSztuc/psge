project(Sandbox)
set(NAME Sandbox)

# Get all the game sources
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/Sources/*.cpp)

# Add the game executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the engine library
target_link_libraries(${PROJECT_NAME} PintSizedGameEngine)