cmake_minimum_required(VERSION 3.22.1)
project(FeatureExtraction)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)

add_executable(FeatureExtraction main.cpp
        include/cxxopts/cxxopts.hpp
        include/indicators/block_progress_bar.hpp include/indicators/cursor_control.hpp
        src/FileHanding.h src/Types.h src/WindowGeneration.h src/SeriesUtils.h src/InformationGain.h src/FeatureFinding.h src/Logger.h
        src/ArgumentParser.h src/FeatureUtils.h src/attributes/Attribute.h src/Feature.h src/attributes/Frequency.h src/attributes/MinDist.h src/attributes/RelFrequency.h)
target_include_directories(FeatureExtraction PRIVATE include)