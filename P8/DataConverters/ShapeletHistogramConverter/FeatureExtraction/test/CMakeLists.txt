cmake_minimum_required(VERSION 3.13)

# Setup Catch2 (download if needed)
Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_SHALLOW    TRUE
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)

# Make sure package is installed
find_package(Catch2 REQUIRED)

# Find all .cpp files in the test folder
set (WILDCARD_SOURCE_CPP *.cpp)
file(GLOB_RECURSE FEATUREEXTRACTION_TEST_SRC_FILES RELATIVE ${CMAKE_SOURCE_DIR}/test ${WILDCARD_SOURCE_CPP})

# Setup executable
add_executable(all_tests ${FEATUREEXTRACTION_TEST_SRC_FILES})
target_include_directories(all_tests PRIVATE ../include ../src)
target_link_libraries(all_tests PRIVATE Catch2::Catch2WithMain)

# Initialise test libraries
include(CTest)
enable_testing()
include(Catch)
catch_discover_tests(all_tests)