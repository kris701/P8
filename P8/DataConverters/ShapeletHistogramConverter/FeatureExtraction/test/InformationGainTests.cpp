#include "core/InformationGain.h"
#include <catch2/catch_test_macros.hpp>
#include "catch2/generators/catch_generators.hpp"

namespace InformationGainTests {
    using namespace InformationGain;

#pragma region CalculateEntropy

    TEST_CASE("InformationGain.CalculateEntropy.Can_GiveCorrectEntropy_EqualValues_1", "[InformationGain]") {
        auto classCount = GENERATE(1, 2, 4, 8, 16, 32);
        auto instanceCount = GENERATE(1, 2, 4, 8, 16, 32);
        int total = 0;
        ClassCount values;
        for (int i = 0; i < classCount; i++) {
            values[i] = instanceCount;
            total += instanceCount;
        }

        double expectedEntropy = 0;

        auto resultingEntropy = CalculateEntropy(total, values);

        INFO("Class Count: " << classCount);
        INFO("Instance Count: " << instanceCount);
        REQUIRE(expectedEntropy == resultingEntropy);
    }

#pragma endregion
}

