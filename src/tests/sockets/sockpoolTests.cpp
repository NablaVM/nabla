/*
    This is a test of the socket pool, not the sockets themselves.
    Socket functionality isn't tested here, just the pool operations
*/

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

        CHECK_EQUAL(capacity, sockpool_get_capacity(sp));

        // Get the 'size' at least 1, at-most capacity
        uint16_t size = getRandom16(1, capacity);

        std::vector<uint16_t> generated;

        // Add some items
        for(uint16_t item = 0; item < size; item++)
        {
            int result = -255;

            uint16_t n = sockpool_create_socket(sp, AF_INET, SOCK_STREAM, 0, (char*)"127.0.0.1", item, 1, &result);

            CHECK_EQUAL(0, result);

            generated.push_back(n);
        }

        // Ensure they were added
        CHECK_EQUAL(size, sockpool_get_size(sp));

        // Go through added items and ensure that they were all created
        for(auto &el : generated)
        {
            CHECK_TRUE(sockpool_get_socket(sp, el) != nullptr);
        }

        // Delete all of them
        for(uint16_t x = 0; x < size; x++)
        {
            sockpool_delete_socket(sp, x);

            // Make sure that it was released
            CHECK_TRUE(sockpool_get_socket(sp, x) == nullptr);

            // Ensure that the size is whats expected
            CHECK_EQUAL(size - x -1, sockpool_get_size(sp));
        }
        
        // Ensure they are all cleared
        CHECK_EQUAL(0, sockpool_get_size(sp));

        // Clear the sock pool
        sockpool_delete(sp);
    }
}