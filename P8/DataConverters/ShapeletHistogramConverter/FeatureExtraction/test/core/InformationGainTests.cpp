#include "core/InformationGain.h"
#include <catch2/catch_test_macros.hpp>
#include "catch2/generators/catch_generators.hpp"

namespace InformationGainTests {
    using namespace InformationGain;

#pragma region CalculateEntropy

    // a = 10/30 = 0.33
    // b = 5/30  = 0.16
    // c = 15/30 = 0.50
    // entropy = (-a * log2(a))       + (-b * log2(b))       + (-c * log2(c)) 
    //           (-0.33 * log2(0.33)) + (-0.16 * log2(0.16)) + (-0.50 * log2(0.50))
    //           (-0.33 * -1.5995)    + (-0.16 * -2.644)     + (-0.50 * -1)
    //           (0.527835)           + (0.42304)            + (0.5)
    //         = 1.450875
    TEST_CASE("InformationGain.CalculateEntropy.Can_GiveCorrectEntropy_EqualValues_1", "[InformationGain]") {
        int total = 30;
        ClassCount values{ 10, 5, 15 };

        double expectedEntropy = 1.450875;

        auto resultingEntropy = CalculateEntropy(total, values);

        // Round expected values, since decimal points is not exactly the same between manual and method result
        REQUIRE(std::round(expectedEntropy * 10) == std::round(resultingEntropy * 10));
    }

    // a = 10/30 = 0.33
    // b = 5/30  = 0.16
    // c = 15/30 = 0.50
    // entropy = (-a * log2(a))       + (-b * log2(b))       + (-c * log2(c)) 
    //           (-0.33 * log2(0.33)) + (-0.16 * log2(0.16)) + (-0.50 * log2(0.50))
    //           (-0.33 * -1.5995)    + (-0.16 * -2.644)     + (-0.50 * -1)
    //           (0.527835)           + (0.42304)            + (0.5)
    //         = 1.450875
    TEST_CASE("InformationGain.CalculateEntropy.Can_GiveCorrectEntropy_EqualValues_1_NoTotal", "[InformationGain]") {
        ClassCount values{ 10, 5, 15 };

        double expectedEntropy = 1.450875;

        auto resultingEntropy = CalculateEntropy(values);

        // Round expected values, since decimal points is not exactly the same between manual and method result
        REQUIRE(std::round(expectedEntropy * 10) == std::round(resultingEntropy * 10));
    }

    // a = 10/65 = 0.15
    // b =  5/65 = 0.07
    // c = 50/65 = 0.76
    // entropy = (-a * log2(a))       + (-b * log2(b))       + (-c * log2(c)) 
    //           (-0.15 * log2(0.15)) + (-0.07 * log2(0.07)) + (-0.76 * log2(0.76))
    //           (-0.15 * -2.737)     + (-0.07 * -3.8365)    + (-0.76 * -0.3959)
    //           (0.41055)            + (0.268555)           + (0.300884)
    //         = 0.979989
    TEST_CASE("InformationGain.CalculateEntropy.Can_GiveCorrectEntropy_EqualValues_2", "[InformationGain]") {
        int total = 65;
        ClassCount values{ 10, 5, 0, 50 };

        double expectedEntropy = 0.979989;

        auto resultingEntropy = CalculateEntropy(total, values);

        // Round expected values, since decimal points is not exactly the same between manual and method result
        REQUIRE(std::round(expectedEntropy * 10) == std::round(resultingEntropy * 10));
    }

    // a = 10/65 = 0.15
    // b =  5/65 = 0.07
    // c = 50/65 = 0.76
    // entropy = (-a * log2(a))       + (-b * log2(b))       + (-c * log2(c)) 
    //           (-0.15 * log2(0.15)) + (-0.07 * log2(0.07)) + (-0.76 * log2(0.76))
    //           (-0.15 * -2.737)     + (-0.07 * -3.8365)    + (-0.76 * -0.3959)
    //           (0.41055)            + (0.268555)           + (0.300884)
    //         = 0.979989
    TEST_CASE("InformationGain.CalculateEntropy.Can_GiveCorrectEntropy_EqualValues_2_NoTotal", "[InformationGain]") {
        ClassCount values{ 10, 5, 0, 50 };

        double expectedEntropy = 0.979989;

        auto resultingEntropy = CalculateEntropy(values);

        // Round expected values, since decimal points is not exactly the same between manual and method result
        REQUIRE(std::round(expectedEntropy * 10) == std::round(resultingEntropy * 10));
    }

    TEST_CASE("InformationGain.CalculateEntropy.Cant_CalculateIfTotalIsZero", "[InformationGain]") {
        ClassCount values{ 1,2,3 };

        REQUIRE_THROWS(CalculateEntropy(0, values));
    }

#pragma endregion

#pragma region CalculateSplitEntropy
    TEST_CASE("InformationGain.CalculateSplitEntropy.Can_GetSplitEntropy_1", "[InformationGain]") {
        // With total = 5 we get:
        //    Class count of {1} gives entropy: 0.4643
        //    Class count of {2} gives entropy: 0.5287
        //    Class count of {3} gives entropy: 0.4421
        //    Class count of {4} gives entropy: 0.2575

        double total = 5;
        std::map<double, ClassCount> values = {
            {0, {1}},
            {0.25, {1}},
            {0.50, {1}},
            {0.75, {1}},
            {1, {1}},
        };

        double resultingSplitEntropy1 = CalculateSplitEntropy(values, 0.125);
        //                       Entropy  Probability           Entropy  Probability
        double expectEntropy1 = (0.4643 * ((double)1 / total) + 0.2575 * ((double)4 / total));
        REQUIRE(std::round(resultingSplitEntropy1 * 100) == std::round(expectEntropy1 * 100));

        double resultingSplitEntropy2 = CalculateSplitEntropy(values, 0.375);
        //                       Entropy  Probability           Entropy  Probability
        double expectEntropy2 = (0.5287 * ((double)2 / total) + 0.4421 * ((double)3 / total));
        REQUIRE(std::round(resultingSplitEntropy2 * 100) == std::round(expectEntropy2 * 100));

        double resultingSplitEntropy3 = CalculateSplitEntropy(values, 0.625);
        //                       Entropy  Probability           Entropy  Probability
        double expectEntropy3 = (0.4421 * ((double)3 / total) + 0.5287 * ((double)2 / total));
        REQUIRE(std::round(resultingSplitEntropy3 * 100) == std::round(expectEntropy3 * 100));

        double resultingSplitEntropy4 = CalculateSplitEntropy(values, 0.875);
        //                       Entropy  Probability           Entropy  Probability
        double expectEntropy4 = (0.2575 * ((double)4 / total) + 0.4643 * ((double)1 / total));
        REQUIRE(std::round(resultingSplitEntropy4 * 100) == std::round(expectEntropy4 * 100));
    }

#pragma endregion


}

