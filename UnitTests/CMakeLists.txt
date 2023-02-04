cmake_minimum_required(VERSION 3.16)

# Find and link against the Google Test library.
#find_package(GTest REQUIRED)

enable_testing()

# Add the test sources
set(CORE_SOURCES
  Core/logger.cpp
  Core/stringtests.cpp
  Core/memorymanager.cpp
  Core/queue.cpp
  Core/event.cpp
  Core/timing.cpp
)

add_executable(psge_core_tests ${CORE_SOURCES})

target_link_libraries(psge_core_tests gtest_main)
target_link_libraries(psge_core_tests PintSizedGameEngine)

# Add a custom target for running the unit tests.
add_test(CoreTest psge_core_tests)