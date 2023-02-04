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

find_package(GTest)
if(GTest_FOUND)
    message("Found GTest")
    target_link_libraries(psge_core_tests GTest::GTest GTest::Main)
else()
    message("Did not find GTest. We will compile it instead.")
    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG release-1.11.0
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    target_link_libraries(psge_core_tests gtest_main)
endif(GTest_FOUND)

target_link_libraries(psge_core_tests PintSizedGameEngine)

# Add a custom target for running the unit tests.
add_test(CoreTest psge_core_tests)
