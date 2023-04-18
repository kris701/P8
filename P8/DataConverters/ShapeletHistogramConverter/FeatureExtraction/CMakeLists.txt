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

add_library(Sources INTERFACE
        src/IO/FileHanding.h src/misc/Constants.h src/core/WindowGeneration.h src/utilities/SeriesUtils.h src/core/InformationGain.h src/core/feature_finding/FeatureFinding.h src/IO/Logger.h
        src/IO/ArgumentParser.h src/utilities/FeatureUtils.h src/utilities/ClassCountUtils.h src/core/attributes/Attribute.h src/types/Feature.h src/core/attributes/Frequency.h src/core/attributes/MinDist.h src/core/attributes/AttributeBuilder.h src/core/feature_finding/Evaluation.h
        src/core/attributes/MaxDist.h src/core/attributes/OccPos.h src/core/DataAugmentation.h)

add_executable(FeatureExtraction main.cpp include/cxxopts/cxxopts.hpp)
target_include_directories(FeatureExtraction PRIVATE include src)

include(CTest)

enable_testing()
add_subdirectory(test)
