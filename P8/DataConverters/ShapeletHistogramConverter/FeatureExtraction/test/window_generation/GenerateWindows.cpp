#include "core/WindowGeneration.h"
#include <catch2/catch_test_macros.hpp>
#include "catch2/generators/catch_generators.hpp"

TEST_CASE("Correct Window Count", "[WindowGeneration]") {
    auto seriesLength = GENERATE(0, 1, 2, 4, 8, 16, 32);
    auto windowSize = GENERATE(1, 2, 4, 8, 16, 32);

    uint expectedWindowCount = 0;
    for (uint i = 0; i + windowSize <= seriesLength; i++)
        expectedWindowCount++;

    Series series;
    for (uint i = 0; i < seriesLength; i++)
        series.push_back((double) 1 / i);

    auto windows = WindowGeneration::GenerateWindows(series, windowSize);

    INFO("Series Length: " << seriesLength);
    INFO("Window Size: " << windowSize);
    REQUIRE(expectedWindowCount == windows.size());
}

