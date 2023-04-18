cmake_minimum_required(VERSION 3.5)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.2 # or a later release
)
FetchContent_GetProperties(Catch2)
if(NOT Catch2_POPULATED)
    FetchContent_Populate(Catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    #add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
endif() 

set (WILDCARD_SOURCE *.cpp)

file(GLOB_RECURSE TEST_SOURCES ${WILDCARD_SOURCE})

add_executable(${TARGET_TEST} ${TEST_SOURCES})

find_package(Catch2 REQUIRED)

# Link to the desired libraries
target_link_libraries(${TARGET_TEST}
    PRIVATE
    Catch2::Catch2
)

include(Catch)
catch_discover_tests(${TARGET_TEST})