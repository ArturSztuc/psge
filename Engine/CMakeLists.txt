# Project names
set(NAME PintSizedGameEngine)
project(${NAME} VERSION 0.0.1)

# Linking the dependencies
include(${CMAKE_SOURCE_DIR}/Externals/CMakeLists.txt)

include(CMakePackageConfigHelpers)
include(GenerateExportHeader)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/sources/*.cpp)

file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/include/*.hpp
                          ${PROJECT_SOURCE_DIR}/include/*.h
)

# Engine compiled as a library
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
generate_export_header(${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PROJECT_VERSION})

# Link the directories
target_include_directories(${PROJECT_NAME} 
                            PUBLIC 
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                              $<INSTALL_INTERFACE:include>
                              ${GLFW_DIR}/include
                              ${Vulkan_INCLUDE_DIRS}
                            PRIVATE
                              ${CMAKE_CURRENT_SOURCE_DIR}/sources
                          )

# Link the librarie
target_link_libraries(${PROJECT_NAME} 
          glfw ${GLFW_LIBRARIES}
          nlohmann_json::nlohmann_json
          Boost::filesystem
          ${Vulkan_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

# Set include directories for PintSizedGameEngine
set(PintSizedGameEngine_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake @ONLY)

# Configure the config file for export
configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${PROJECT_NAME}/cmake/${PROJECT_NAME}"
)

# Create the version file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# Install the game engine library and header files
install(TARGETS ${PROJECT_NAME} 
        EXPORT ${PROJECT_NAME}Config
        ARCHIVE DESTINATION  ${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/
        LIBRARY DESTINATION  ${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/
        RUNTIME DESTINATION  ${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}/
        INCLUDES DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}/
      )

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})

# Export the library
install(EXPORT ${PROJECT_NAME}Config 
        FILE ${PROJECT_NAME}Config.cmake
        NAMESPACE psge:: 
        DESTINATION ${PROJECT_NAME}/${PROJECT_NAME}
      )

# Set the assets directort
set(ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

# Copy assets to the build directory
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${ASSET_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

# Add a custom target so assets are copied before building the library
add_custom_target(${PROJECT_NAME}_assets DEPENDS ${ASSET_DIR})

# Link the assets target to the engine target
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_assets)

# install config and version file for the library
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY ${ASSET_DIR} DESTINATION ${PROJECT_NAME})

set(ASSETS_DIR "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/assets/")
add_compile_definitions(ASSETS_DIR="${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/assets/")
add_definitions(-DASSETS_DIR="${ASSETS_DIR}")


# Build & install shaders

# Find program to build shaders
find_program(GLSL_VALIDATOR glslangValidator HINTS
  ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
  /usr/bin
  /usr/local/bin
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  $ENV{VULKAN_SDK}/Bin/
  $ENV{VULKAN_SDK}/Bin32/
)

# get all fragment & vertex shader files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  ${PROJECT_SOURCE_DIR}/assets/shaders/*.frag
  ${PROJECT_SOURCE_DIR}/assets/shaders/*.vert
)

# Create commands for each shader
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/assets/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

# Add new custom target, Shaders
add_custom_target(
  Shaders 
  DEPENDS ${SPIRV_BINARY_FILES}
)

# Make our main project depend on these shaders
add_dependencies(${PROJECT_NAME} Shaders)

# Copying from build folder to shaders folder. Maybe not needed...
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/assets/shaders"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
)

#install(FILES ${ASSET_DIR}
#  DESTINATION ${PROJECT_NAME}/assets
#)
