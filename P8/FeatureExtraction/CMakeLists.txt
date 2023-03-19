cmake_minimum_required(VERSION 3.24)
project(FeatureExtraction)

set(CMAKE_CXX_STANDARD 23)

add_executable(FeatureExtraction main.cpp
        include/cxxopts/cxxopts.hpp
        include/indicators/block_progress_bar.hpp include/indicators/cursor_control.hpp
        src/FileHanding.h src/Types.h src/WindowGeneration.h src/SeriesUtils.h src/InformationGain.h src/FeatureFinding.h src/Attributes.h src/Logger.h
        )
target_include_directories(FeatureExtraction PRIVATE include)