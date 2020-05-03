
extern "C"
{
    #include "sockpool.h"
}

#include <iostream>
#include <random>
#include "CppUTest/TestHarness.h"

namespace
{
    uint16_t getRandom16(uint16_t low, uint16_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(low, high);
        return dis(gen);
    }
}

TEST_GROUP(SocketPoolTests)
{   
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(SocketPoolTests, sockpoolTests)
{
    for(int i = 0; i < 10; i++)
    {
        // Make sure its at least size 10
        uint16_t capacity = getRandom16(10, 255); // Full capacity is too big for tests

        sockpool * sp = sockpool_create(capacity);

        CHECK_TRUE(sp != NULL);

        // Get the 'size' at least 1, at-most capacity
        uint16_t size = getRandom16(1, capacity);

        for(uint16_t item = 0; item < size; item++)
        {
            int result = -255;

            // Need to randomize the creaton parameters
            //

            uint16_t n = sockpool_create_socket(sp, AF_INET, SOCK_STREAM, 0, (char*)"127.0.0.1", item, 1, &result);

            // Create a list to store all 'n' in. Then we can loop over them after creation and ensure that 
            // the 'get' methods all match up to what we expect. - This will mean we need to save the randomized
            // creation info


            CHECK_EQUAL(0, result);
        }

        // Clear the sock pool
        sockpool_delete(sp);
    }
}