cmake_minimum_required(VERSION 3.5)
project(FeatureExtraction)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
Include(FetchContent)

FetchContent_Declare(
    cxxopts 
    URL https://raw.githubusercontent.com/jarro2783/cxxopts/v3.1.1/include/cxxopts.hpp
    DOWNLOAD_NO_EXTRACT TRUE
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/include/cxxopts
)
FetchContent_MakeAvailable(cxxopts)

add_subdirectory(src)

include(CTest)
add_subdirectory(test)

add_executable(FeatureExtraction main.cpp include/cxxopts/cxxopts.hpp)
target_include_directories(FeatureExtraction PRIVATE include src)
