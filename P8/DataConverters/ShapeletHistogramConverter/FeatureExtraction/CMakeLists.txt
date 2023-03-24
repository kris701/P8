cmake_minimum_required(VERSION 3.22.1)
project(FeatureExtraction)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_library(Sources INTERFACE
        src/FileHanding.h src/Types.h src/WindowGeneration.h src/SeriesUtils.h src/InformationGain.h src/FeatureFinding.h src/Logger.h
        src/ArgumentParser.h src/FeatureUtils.h src/attributes/Attribute.h src/Feature.h src/attributes/Frequency.h src/attributes/MinDist.h src/attributes/RelFrequency.h)

add_executable(FeatureExtraction main.cpp
        include/cxxopts/cxxopts.hpp
        include/indicators/block_progress_bar.hpp include/indicators/cursor_control.hpp
        )
target_include_directories(FeatureExtraction PRIVATE include)


include(CTest)
include(Catch)
include(test/CMakeLists.txt)
