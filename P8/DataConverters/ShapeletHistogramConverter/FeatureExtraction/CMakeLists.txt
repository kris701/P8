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
FetchContent_GetProperties(Catch2)
if(NOT Catch2_POPULATED)
    FetchContent_Populate(Catch2)
    add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
endif() 

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
        src/core/attributes/MaxDist.h src/core/attributes/OccPos.h src/Preprocessing/DataAugmentation.h)

add_executable(FeatureExtraction main.cpp include/cxxopts/cxxopts.hpp src/Preprocessing/DataPurge.h src/Preprocessing/DataSplit.h)
target_include_directories(FeatureExtraction PRIVATE include src)

include(CTest)
include(Catch)
include(test/CMakeLists.txt)
