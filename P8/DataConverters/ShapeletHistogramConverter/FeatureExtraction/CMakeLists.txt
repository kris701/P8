cmake_minimum_required(VERSION 3.13)
project(FeatureExtraction)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)

# Fetch the cxxopts library
Include(FetchContent)
FetchContent_Declare(
    cxxopts 
    URL https://raw.githubusercontent.com/jarro2783/cxxopts/v3.1.1/include/cxxopts.hpp
    DOWNLOAD_NO_EXTRACT TRUE
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/include/cxxopts
)
FetchContent_MakeAvailable(cxxopts)

# Setup src library
add_subdirectory(src)

# Setup tests with catch2
include(CTest)
add_subdirectory(test)

# Make final executable
add_executable(FeatureExtraction main.cpp include/cxxopts/cxxopts.hpp)
target_include_directories(FeatureExtraction PRIVATE include src)
