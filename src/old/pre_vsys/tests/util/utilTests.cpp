
extern "C"
{
    #include "util.h"
}

#include <iostream>
#include <random>
#include "CppUTest/TestHarness.h"

namespace
{
    struct ExtractionTestCase
    {
        uint16_t  expected;
        uint8_t  idx;
        uint64_t data;
    };

    std::vector<ExtractionTestCase> extractionCases8 {
        { 0xFA, 7, 0xFAFFFFFFFFFFFFFF },
        { 0xAA, 6, 0xFFAAFFFFFFFFFFFF },
        { 0xBB, 5, 0xFFFFBBFFFFFFFFFF },
        { 0xDA, 4, 0xFFFFFFDAFFFFFFFF },
        { 0xCC, 3, 0xFFFFFFFFCCFFFFFF },
        { 0x93, 2, 0xFFFFFFFFFF93FFFF },
        { 0xEE, 1, 0xFFFFFFFFFFFFEEFF },
        { 0xEA, 0, 0xFFFFFFFFFFFFFFEA }
    };

    std::vector<ExtractionTestCase> extractionCases16 {
        { 0xFACC, 7, 0xFACCFFFFFFFFFFFF },
        { 0xAACC, 6, 0xFFAACCFFFFFFFFFF },
        { 0xBBCC, 5, 0xFFFFBBCCFFFFFFFF },
        { 0xDACC, 4, 0xFFFFFFDACCFFFFFF },
        { 0xCCCC, 3, 0xFFFFFFFFCCCCFFFF },
        { 0x93CC, 2, 0xFFFFFFFFFF93CCFF },
        { 0xEECC, 1, 0xFFFFFFFFFFFFEECC },
    };

    // Double stuff is basically a copy of how util does its work at the time of this test creation.
    // However, we know its correct now, so if these ever fail its because util was changed
    // and things are most definitely broken in the vm now
    union doubleEval
    {
        uint64_t val;
        double d;
    };

    double test_util_convert_uint64_to_double(uint64_t val)
    {
        union doubleEval d; d.val = val;
        return d.d;
    }

    uint64_t test_util_convert_double_to_uint64(double val)
    {
        union doubleEval d; d.d = val;
        return d.val;
    }

    bool test_util_check_double_equal(double lhs, double rhs)
    {
        double precision = 0.00001;
        if (((lhs - precision) < rhs) && 
            ((lhs + precision) > rhs))
        {
            return true;
        }
        return false;
    }

    double getRandomDouble(double low, double high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(low, high);
        return dis(gen);
    }
}


TEST_GROUP(UtilityTests)
{   
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(UtilityTests, extraction)
{
    for(auto &tc : extractionCases8)
    {
        CHECK_EQUAL_TEXT(tc.expected, util_extract_byte(tc.data, tc.idx), "Utility failed to extract 1 byte");
    }

    for(auto &tc : extractionCases16)
    {
        CHECK_EQUAL_TEXT(tc.expected, util_extract_two_bytes(tc.data, tc.idx), "Utility failed to extract 2 bytes");
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(UtilityTests, conversion)
{
    for(int i = 0; i < 100; i++)
    {
        double val = getRandomDouble(-10000.0, 10000.0);

        uint64_t t64 = test_util_convert_double_to_uint64(val);
        uint64_t u64 = util_convert_double_to_uint64(val);

        CHECK_EQUAL_TEXT(t64, u64, "Converted doubles don't match");

        double tdouble = test_util_convert_uint64_to_double(t64);
        double udouble = util_convert_uint64_to_double(u64);

        CHECK_EQUAL_TEXT(val, tdouble, "The test conversion of double -> int -> double failed. Don't trust anyone. Call the cops.");

        // This double equals should be legit as they are suppose to be EXACTLY equal.
        CHECK_EQUAL_TEXT(tdouble, udouble, "Converted uint64_ts don't match");
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(UtilityTests, equality)
{
    for(double i = 1.0; i < 25.0; i += 0.45)
    {
        double d = getRandomDouble(i, i + 1.0);

        CHECK_EQUAL_TEXT( test_util_check_double_equal(i, d), util_check_double_equal(i, d), "Result of util double equality check didn't meet expectation");
    }
}

