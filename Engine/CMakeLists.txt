# Project names
set(NAME PintSizedGameEngine)
project(${NAME} VERSION 0.0.1)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/*.hpp
                          ${PROJECT_SOURCE_DIR}/*.h
)

# Engine compiled as a library
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link the directories
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR} ${GLFW_DIR}/include
)

# Link the libraries
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} nlohmann_json::nlohmann_json)
