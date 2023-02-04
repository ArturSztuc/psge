# Enables testing --required for GTest framework to work
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

# Adds an executable to compile
add_executable(psge_core_tests ${CORE_SOURCES})

# Link the engine and GTest dependencies
target_link_libraries(psge_core_tests GTest::gtest_main)
target_link_libraries(psge_core_tests PintSizedGameEngine)

# Add a custom target for running the unit tests.
add_test(CoreTest psge_core_tests)
