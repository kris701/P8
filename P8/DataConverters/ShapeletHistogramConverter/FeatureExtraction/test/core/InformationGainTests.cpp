#include "core/InformationGain.hpp"
#include <catch2/catch_test_macros.hpp>
#include "catch2/generators/catch_generators.hpp"

namespace InformationGainTests {
    using namespace InformationGain;

    // Note, some of these tests have approximate equals, since the entropy values can change quite a lot with just a little reduced decimal count

#pragma region Helpers

    double RoundTo(double value, int decimal) {
        for (int i = 0; i < decimal; i++)
            value *= 10;
        int tempValue = (int)value;
        double tempDoubleValue = (double)tempValue;
        for (int i = 0; i < decimal; i++)
            tempDoubleValue /= 10;
        return tempDoubleValue;
    }

#pragma endregion

#pragma region CalculateEntropy

    int CalculateEntropyDecimalAccuracy = 4;

    // a = 10/30 = 0.33
    // b = 5/30  = 0.16
    // c = 15/30 = 0.50
    // entropy = (-a * log2(a))       + (-b * log2(b))       + (-c * log2(c)) 
    //           (-0.33 * log2(0.33)) + (-0.16 * log2(0.16)) + (-0.50 * log2(0.50))
    //           (-0.33 * -1.5995)    + (-0.16 * -2.644)     + (-0.50 * -1)
    //           (0.527835)           + (0.42304)            + (0.5)
    //        ~= 1.4591
    TEST_CASE("InformationGain.CalculateEntropy.Can_GiveCorrectEntropy_EqualValues_1", "[InformationGain]") {
        int total = 30;
        ClassCount values{ 10, 5, 15 };

        double expectedEntropy = 1.4591;

        auto resultingEntropy = CalculateEntropy(total, values);

        // Round expected values, since decimal points is not exactly the same between manual and method result
        REQUIRE(RoundTo(expectedEntropy, CalculateEntropyDecimalAccuracy) == RoundTo(resultingEntropy, CalculateEntropyDecimalAccuracy));
    }

    // a = 10/30 = 0.33
    // b = 5/30  = 0.16
    // c = 15/30 = 0.50
    // entropy = (-a * log2(a))       + (-b * log2(b))       + (-c * log2(c)) 
    //           (-0.33 * log2(0.33)) + (-0.16 * log2(0.16)) + (-0.50 * log2(0.50))
    //           (-0.33 * -1.5995)    + (-0.16 * -2.644)     + (-0.50 * -1)
    //           (0.527835)           + (0.42304)            + (0.5)
    //        ~= 1.4591
    TEST_CASE("InformationGain.CalculateEntropy.Can_GiveCorrectEntropy_EqualValues_1_NoTotal", "[InformationGain]") {
        ClassCount values{ 10, 5, 15 };

        double expectedEntropy = 1.4591;

        auto resultingEntropy = CalculateEntropy(values);

        // Round expected values, since decimal points is not exactly the same between manual and method result
        REQUIRE(RoundTo(expectedEntropy, CalculateEntropyDecimalAccuracy) == RoundTo(resultingEntropy, CalculateEntropyDecimalAccuracy));
    }

    // a = 10/65 = 0.15
    // b =  5/65 = 0.07
    // c = 50/65 = 0.76
    // entropy = (-a * log2(a))       + (-b * log2(b))       + (-c * log2(c)) 
    //           (-0.15 * log2(0.15)) + (-0.07 * log2(0.07)) + (-0.76 * log2(0.76))
    //           (-0.15 * -2.737)     + (-0.07 * -3.8365)    + (-0.76 * -0.3959)
    //           (0.41055)            + (0.268555)           + (0.300884)
    //        ~= 0.9912
    TEST_CASE("InformationGain.CalculateEntropy.Can_GiveCorrectEntropy_EqualValues_2", "[InformationGain]") {
        int total = 65;
        ClassCount values{ 10, 5, 0, 50 };

        double expectedEntropy = 0.9912;

        auto resultingEntropy = CalculateEntropy(total, values);

        // Round expected values, since decimal points is not exactly the same between manual and method result
        REQUIRE(RoundTo(expectedEntropy, CalculateEntropyDecimalAccuracy) == RoundTo(resultingEntropy, CalculateEntropyDecimalAccuracy));
    }

    // a = 10/65 = 0.15
    // b =  5/65 = 0.07
    // c = 50/65 = 0.76
    // entropy = (-a * log2(a))       + (-b * log2(b))       + (-c * log2(c)) 
    //           (-0.15 * log2(0.15)) + (-0.07 * log2(0.07)) + (-0.76 * log2(0.76))
    //           (-0.15 * -2.737)     + (-0.07 * -3.8365)    + (-0.76 * -0.3959)
    //           (0.41055)            + (0.268555)           + (0.300884)
    //        ~= 0.979989
    TEST_CASE("InformationGain.CalculateEntropy.Can_GiveCorrectEntropy_EqualValues_2_NoTotal", "[InformationGain]") {
        ClassCount values{ 10, 5, 0, 50 };

        double expectedEntropy = 0.9912;

        auto resultingEntropy = CalculateEntropy(values);

        // Round expected values, since decimal points is not exactly the same between manual and method result
        REQUIRE(RoundTo(expectedEntropy, CalculateEntropyDecimalAccuracy) == RoundTo(resultingEntropy, CalculateEntropyDecimalAccuracy));
    }

    TEST_CASE("InformationGain.CalculateEntropy.Cant_CalculateIfTotalIsZero", "[InformationGain]") {
        ClassCount values{ 1,2,3 };

        REQUIRE_THROWS(CalculateEntropy(0, values));
    }

#pragma endregion

#pragma region CalculateSplitEntropy

    int CalculateSplitEntropyDecimalAccuracy = 3;

    TEST_CASE("InformationGain.CalculateSplitEntropy.Can_GetSplitEntropy_1", "[InformationGain]") {
        // With total = 5 we get:
        //    Class count of {1} gives entropy: 0.4643
        //    Class count of {2} gives entropy: 0.5287
        //    Class count of {3} gives entropy: 0.4421
        //    Class count of {4} gives entropy: 0.2575

        double total = 5;
        ValueLine values = ValueLine({
                                             {0,    {1}},
                                             {0.25, {1}},
                                             {0.50, {1}},
                                             {0.75, {1}},
                                             {1,    {1}},
                                     });

        double resultingSplitEntropy1 = CalculateSplitEntropy(values, 0.125);
        //                       Entropy  Probability           Entropy  Probability
        double expectEntropy1 = (0.4643 * ((double)1 / total) + 0.2575 * ((double)4 / total));
        REQUIRE(RoundTo(resultingSplitEntropy1, CalculateSplitEntropyDecimalAccuracy) == RoundTo(expectEntropy1, CalculateSplitEntropyDecimalAccuracy));

        double resultingSplitEntropy2 = CalculateSplitEntropy(values, 0.375);
        //                       Entropy  Probability           Entropy  Probability
        double expectEntropy2 = (0.5287 * ((double)2 / total) + 0.4421 * ((double)3 / total));
        REQUIRE(RoundTo(resultingSplitEntropy2, CalculateSplitEntropyDecimalAccuracy) == RoundTo(expectEntropy2, CalculateSplitEntropyDecimalAccuracy));

        double resultingSplitEntropy3 = CalculateSplitEntropy(values, 0.625);
        //                       Entropy  Probability           Entropy  Probability
        double expectEntropy3 = (0.4421 * ((double)3 / total) + 0.5287 * ((double)2 / total));
        REQUIRE(RoundTo(resultingSplitEntropy3, CalculateSplitEntropyDecimalAccuracy) == RoundTo(expectEntropy3, CalculateSplitEntropyDecimalAccuracy));

        double resultingSplitEntropy4 = CalculateSplitEntropy(values, 0.875);
        //                       Entropy  Probability           Entropy  Probability
        double expectEntropy4 = (0.2575 * ((double)4 / total) + 0.4643 * ((double)1 / total));
        REQUIRE(RoundTo(resultingSplitEntropy4, CalculateSplitEntropyDecimalAccuracy) == RoundTo(expectEntropy4, CalculateSplitEntropyDecimalAccuracy));
    }

#pragma endregion

#pragma region GetOptimalSplitPoint

    int GetOptimalSplitPointDecimalAccuracy = 3;

    TEST_CASE("InformationGain.GetOptimalSplitPoint.Can_GetOptimalSplitPoint_1", "[InformationGain]") {
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

        // The first split, between 0 and 0.25 (i.e. 0.125) is the best point, since the entropy is the lowest here.
        // Do note, that technically the split point between 0.75 and 1 is equally as good.
        double expectedBestPoint = (0 + 0.25) / 2;
        double bestPoint = GetOptimalSplitPoint(values);

        REQUIRE(RoundTo(bestPoint, GetOptimalSplitPointDecimalAccuracy) == RoundTo(expectedBestPoint, GetOptimalSplitPointDecimalAccuracy));
    }

    TEST_CASE("InformationGain.GetOptimalSplitPoint.Can_GetOptimalSplitPoint_2", "[InformationGain]") {
        // With total = 5 we get:
        //    Class count of {1} gives entropy: 0.4643
        //    Class count of {2} gives entropy: 0.5287
        //    Class count of {3} gives entropy: 0.4421
        //    Class count of {4} gives entropy: 0.2575

        double total = 5;
        std::map<double, ClassCount> values = {
            {0, {10}},
            {0.25, {1}},
            {0.50, {1}},
            {0.75, {1}},
            {1, {1}},
        };

        // This time (compared to the previous test) the best split point is now the last point (they where equally as good before)
        double expectedBestPoint = (0.75 + 1) / 2;
        double bestPoint = GetOptimalSplitPoint(values);

        REQUIRE(RoundTo(bestPoint, GetOptimalSplitPointDecimalAccuracy) == RoundTo(expectedBestPoint, GetOptimalSplitPointDecimalAccuracy));
    }

    TEST_CASE("InformationGain.GetOptimalSplitPoint.Cant_SplitSinglePoint", "[InformationGain]") {
        std::map<double, ClassCount> values = {
            {0, {1}}
        };
        REQUIRE_THROWS(GetOptimalSplitPoint(values));
    }

#pragma endregion

#pragma region CalculateInformationGain

    int CalculateInformationGainDecimalAccuracy = 3;

    TEST_CASE("InformationGain.CalculateInformationGain.Can_CalculateInformationGain_SameWithZeroPriorEntropy_1", "[InformationGain]") {
        double total = 5;
        std::map<double, ClassCount> values = {
            {0, {1}},
            {0.25, {1}},
            {0.50, {1}},
            {0.75, {1}},
            {1, {1}},
        };

        double bestGain = CalculateInformationGain(values, 0);

        REQUIRE(bestGain == 0);
    }

    TEST_CASE("InformationGain.CalculateInformationGain.Can_CalculateInformationGain", "[InformationGain]") {
        auto priorEntropy = GENERATE(1, 2, 4, 8);
        double total = 5;
        std::map<double, ClassCount> values = {
            {0, {1}},
            {0.25, {1}},
            {0.50, {1}},
            {0.75, {1}},
            {1, {1}},
        };

        double expectEntropy1 = (0.4643 * ((double)1 / total) + 0.2575 * ((double)4 / total));
        double expectedBestGain = priorEntropy - expectEntropy1;
        double bestGain = CalculateInformationGain(values, priorEntropy);

        INFO("Prior Entropy: " << priorEntropy);
        REQUIRE(RoundTo(bestGain, CalculateInformationGainDecimalAccuracy) == RoundTo(expectedBestGain, CalculateInformationGainDecimalAccuracy));
    }

#pragma endregion
}

