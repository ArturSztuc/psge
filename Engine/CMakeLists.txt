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
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
generate_export_header(${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PROJECT_VERSION})

# Link the directories
target_include_directories(${PROJECT_NAME} 
                            PUBLIC 
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                              $<INSTALL_INTERFACE:include>
                              ${GLFW_DIR}/include
                            PRIVATE
                              ${CMAKE_CURRENT_SOURCE_DIR}/src
                          )

# Link the librarie
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} nlohmann_json::nlohmann_json Boost::filesystem)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

# Install the game engine library and header files
install(TARGETS ${PROJECT_NAME} 
        EXPORT ${PROJECT_NAME}Config
        ARCHIVE DESTINATION lib/${PROJECT_NAME}/lib
        LIBRARY DESTINATION lib/${PROJECT_NAME}/lib
        RUNTIME DESTINATION lib/${PROJECT_NAME}/bin
        INCLUDES DESTINATION lib/${PROJECT_NAME}/include
      )

install(DIRECTORY include/ DESTINATION lib/${PROJECT_NAME}/include)

# Export the library
install(EXPORT ${PROJECT_NAME}Config 
        FILE ${PROJECT_NAME}Config.cmake
        NAMESPACE psge:: 
        DESTINATION lib/${PROJECT_NAME}/cmake/${PROJECT_NAME}
      )

# Configure the config file for export
configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "lib/${PROJECT_NAME}/cmake/${PROJECT_NAME}"
)

# Create the version file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
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
  DESTINATION lib/${PROJECT_NAME}/cmake/${PROJECT_NAME}
)

install(DIRECTORY ${ASSET_DIR} DESTINATION lib/${PROJECT_NAME})

set(ASSETS_DIR "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/assets/")
add_compile_definitions(ASSETS_DIR="${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/assets/")
add_definitions(-DASSETS_DIR="${ASSETS_DIR}")

#install(FILES ${ASSET_DIR}
#  DESTINATION lib/${PROJECT_NAME}/assets
#)
