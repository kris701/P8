#include "core/WindowGeneration.h"
#include <catch2/catch_test_macros.hpp>
#include "catch2/generators/catch_generators.hpp"

namespace WindowGenerationTests {
    using namespace WindowGeneration;

#pragma region GenerateWindowsOfLength

    TEST_CASE("Can_GenerateWindowsOfLength_GiveCorrectWindows", "[WindowGeneration]") {
        auto seriesLength = GENERATE(1, 2, 4, 8, 16, 32);
        auto windowSize = GENERATE(1, 2, 4, 8, 16, 32);

        uint expectedWindowCount = 0;
        for (uint i = 0; i + windowSize <= seriesLength; i++)
            expectedWindowCount++;

        Series series;
        for (uint i = 0; i < seriesLength; i++)
            series.push_back((double)1 / i);

        auto windows = GenerateWindowsOfLength(series, windowSize);

        INFO("Series Length: " << seriesLength);
        INFO("Window Size: " << windowSize);
        REQUIRE(expectedWindowCount == windows.size());
    }

    TEST_CASE("Can_GenerateWindowsOfLength_GiveNoneIfSeriesEmpty", "[WindowGeneration]") {
        auto windowSize = GENERATE(1, 2, 4, 8, 16, 32);

        Series series;

        auto windows = GenerateWindowsOfLength(series, windowSize);

        INFO("Window Size: " << windowSize);
        REQUIRE(0 == windows.size());
    }

    TEST_CASE("Can_GenerateWindowsOfLength_GiveNoneIfLengthZero", "[WindowGeneration]") {
        auto seriesLength = GENERATE(0, 1, 2, 4, 8, 16, 32);

        Series series;
        for (uint i = 0; i < seriesLength; i++)
            series.push_back((double)1 / i);

        auto windows = GenerateWindowsOfLength(series, 0);

        INFO("Series Length: " << seriesLength);
        REQUIRE(0 == windows.size());
    }

#pragma endregion

#pragma region GenerateWindowsOfMinMaxLength

    TEST_CASE("Can_GenerateWindowsOfMinMaxLength_GiveCorrectWindows", "[WindowGeneration]") {
        auto seriesLength = GENERATE(1, 2, 4, 8);
        auto minWindowSize = GENERATE(1, 2, 4, 8);
        auto maxWindowSize = GENERATE(1, 2, 4, 8);

        uint expectedWindowCount = 0;
        for (uint i = minWindowSize; i <= maxWindowSize; i++)
            for (uint j = 0; j + i <= seriesLength; j++)
                expectedWindowCount++;

        Series series;
        for (uint i = 0; i < seriesLength; i++)
            series.push_back((double)1 / i);

        auto windows = GenerateWindowsOfMinMaxLength(series, minWindowSize, maxWindowSize);

        INFO("Series Length: " << seriesLength);
        INFO("Min Window Size: " << minWindowSize);
        INFO("Max Window Size: " << maxWindowSize);
        REQUIRE(expectedWindowCount == windows.size());
    }

    TEST_CASE("Can_GenerateWindowsOfMinMaxLength_SetsMaxWindowSizeToSeriesLengthIfGivenIsZero", "[WindowGeneration]") {
        auto seriesLength = GENERATE(1, 2, 4, 8);
        auto minWindowSize = GENERATE(1, 2, 4, 8);

        uint expectedWindowCount = 0;
        for (uint i = minWindowSize; i <= seriesLength; i++)
            for (uint j = 0; j + i <= seriesLength; j++)
                expectedWindowCount++;

        Series series;
        for (uint i = 0; i < seriesLength; i++)
            series.push_back((double)1 / i);

        auto windows = GenerateWindowsOfMinMaxLength(series, minWindowSize, 0);

        INFO("Series Length: " << seriesLength);
        INFO("Min Window Size: " << minWindowSize);
        INFO("Max Window Size: " << 0);
        REQUIRE(expectedWindowCount == windows.size());
    }


    TEST_CASE("Can_GenerateWindowsOfMinMaxLength_SetsMaxWindowSizeToSeriesLengthIfGivenIsLarger", "[WindowGeneration]") {
        auto seriesLength = GENERATE(1, 2, 4, 8);
        auto minWindowSize = GENERATE(1, 2, 4, 8);
        auto maxWindowSize = GENERATE(10, 20, 40, 80);

        uint expectedWindowCount = 0;
        for (uint i = minWindowSize; i <= seriesLength; i++)
            for (uint j = 0; j + i <= seriesLength; j++)
                expectedWindowCount++;

        Series series;
        for (uint i = 0; i < seriesLength; i++)
            series.push_back((double)1 / i);

        auto windows = GenerateWindowsOfMinMaxLength(series, minWindowSize, maxWindowSize);

        INFO("Series Length: " << seriesLength);
        INFO("Min Window Size: " << minWindowSize);
        INFO("Max Window Size: " << maxWindowSize);
        REQUIRE(expectedWindowCount == windows.size());
    }

    TEST_CASE("Can_GenerateWindowsOfMinMaxLength_GiveNoneIfSeriesEmpty", "[WindowGeneration]") {
        auto minWindowSize = GENERATE(1, 2, 4, 8);
        auto maxWindowSize = GENERATE(1, 2, 4, 8);

        Series series;

        auto windows = GenerateWindowsOfMinMaxLength(series, minWindowSize, maxWindowSize);

        INFO("Min Window Size: " << minWindowSize);
        INFO("Max Window Size: " << maxWindowSize);
        REQUIRE(0 == windows.size());
    }

    TEST_CASE("Can_GenerateWindowsOfMinMaxLength_GiveNoneIfMinLargerThanMax", "[WindowGeneration]") {
        auto seriesLength = GENERATE(1, 2, 4, 8);
        auto minWindowSize = GENERATE(5, 6, 7, 8);
        auto maxWindowSize = GENERATE(1, 2, 3, 4);

        Series series;
        for (uint i = 0; i < seriesLength; i++)
            series.push_back((double)1 / i);

        auto windows = GenerateWindowsOfMinMaxLength(series, minWindowSize, maxWindowSize);

        INFO("Series Length: " << seriesLength);
        INFO("Min Window Size: " << minWindowSize);
        INFO("Max Window Size: " << maxWindowSize);
        REQUIRE(0 == windows.size());
    }

#pragma endregion

#pragma region RemoveDuplicateWindows

    TEST_CASE("Can_RemoveDuplicateWindows_Correctly", "[WindowGeneration]") {
        auto seriesLength = GENERATE(1, 2, 4, 8);
        auto duplicates = GENERATE(1, 2, 4, 8);

        std::vector<Series> windows;
        for (uint i = 0; i < duplicates; i++) {
            Series series;
            for (uint j = 0; j < seriesLength; j++)
                series.push_back(j);
            windows.push_back(series);
        }

        REQUIRE(duplicates == windows.size());
        RemoveDuplicateWindows(&windows);
        REQUIRE(1 == windows.size());
    }

    TEST_CASE("Cant_RemoveDuplicateWindows_IfNoDuplicates", "[WindowGeneration]") {
        auto seriesLength = GENERATE(1, 2, 4, 8);
        auto duplicates = GENERATE(1, 2, 4, 8);

        std::vector<Series> windows;
        for (uint i = 0; i < duplicates; i++) {
            Series series;
            for (uint j = 0; j < seriesLength; j++)
                series.push_back(i);
            windows.push_back(series);
        }

        REQUIRE(duplicates == windows.size());
        RemoveDuplicateWindows(&windows);
        REQUIRE(duplicates == windows.size());
    }

#pragma endregion

#pragma region GenerateWindowsOfMinMaxLength (LabelledSeries)

    TEST_CASE("Can_GenerateWindowsOfMinMaxLength_GiveCorrectWindowsFromSingle", "[WindowGeneration]") {
        auto seriesLength = GENERATE(1, 2, 4, 8);
        auto minWindowSize = GENERATE(1, 2, 4, 8);
        auto maxWindowSize = GENERATE(1, 2, 4, 8);

        uint expectedWindowCount = 0;
        for (uint i = minWindowSize; i <= maxWindowSize; i++)
            for (uint j = 0; j + i <= seriesLength; j++)
                expectedWindowCount++;

        std::vector<LabelledSeries> labelledSeries;

        Series series;
        for (uint i = 0; i < seriesLength; i++)
            series.push_back(i);
        labelledSeries.push_back(LabelledSeries(0, series));

        auto windows = GenerateWindowsOfMinMaxLength(labelledSeries, minWindowSize, maxWindowSize);

        INFO("Series Length: " << seriesLength);
        INFO("Min Window Size: " << minWindowSize);
        INFO("Max Window Size: " << maxWindowSize);
        REQUIRE(expectedWindowCount == windows.size());
    }

    TEST_CASE("Can_GenerateWindowsOfMinMaxLength_GiveCorrectWindowsFromTwoUnique", "[WindowGeneration]") {
        auto seriesLength = GENERATE(1, 2, 4, 8);
        auto minWindowSize = GENERATE(1, 2, 4, 8);
        auto maxWindowSize = GENERATE(1, 2, 4, 8);

        uint expectedWindowCount = 0;
        for (uint i = minWindowSize; i <= maxWindowSize; i++)
            for (uint j = 0; j + i <= seriesLength; j++)
                expectedWindowCount++;
        expectedWindowCount *= 2;

        std::vector<LabelledSeries> labelledSeries;

        Series series1;
        Series series2;
        for (uint i = 0; i < seriesLength; i++) {
            series1.push_back(i);
            series2.push_back(seriesLength + i);
        }
        labelledSeries.push_back(LabelledSeries(0, series1));
        labelledSeries.push_back(LabelledSeries(1, series2));

        auto windows = GenerateWindowsOfMinMaxLength(labelledSeries, minWindowSize, maxWindowSize);

        INFO("Series Length: " << seriesLength);
        INFO("Min Window Size: " << minWindowSize);
        INFO("Max Window Size: " << maxWindowSize);
        REQUIRE(expectedWindowCount == windows.size());
    }

#pragma endregion
}