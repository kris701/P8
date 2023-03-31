cmake_minimum_required(VERSION 3.22.1)
project(Classifier)
set(CMAKE_CXX_STANDARD 20)

Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1 # or a later release
)

FetchContent_Declare(
    cxxopts 
    URL https://raw.githubusercontent.com/jarro2783/cxxopts/v3.1.1/include/cxxopts.hpp
    DOWNLOAD_NO_EXTRACT TRUE
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/include/cxxopts
)
FetchContent_MakeAvailable(cxxopts)

FetchContent_Declare(
    indicators 
    URL https://raw.githubusercontent.com/p-ranav/indicators/v2.3/single_include/indicators/indicators.hpp
    DOWNLOAD_NO_EXTRACT TRUE
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/include/indicators
)
FetchContent_MakeAvailable(indicators)

add_executable(Classifier main.cpp
        include/cxxopts/cxxopts.hpp
        include/indicators/indicators.hpp src/ArgumentParsing.h src/FileReading.h src/Types.h src/Classification.h)
target_include_directories(Classifier PRIVATE src include)
