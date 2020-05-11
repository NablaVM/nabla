#include "CppUTest/TestHarness.h"

#include "VSysMemory.hpp"

TEST_GROUP(MemoryTests)
{   
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(MemoryTests, oneByte)
{
    NABLA::VSYS::Memory<10> memunit;

    CHECK_FALSE(memunit.hasData());

    for(int i = 0; i < 10; i++)
    {
        CHECK_TRUE(memunit.push_8(i+10));

        uint8_t data = 0;
        CHECK_TRUE(memunit.get_8(i, data));

        CHECK_EQUAL(i+10 , data);

        CHECK_TRUE(memunit.set_8(i, i));
    }

    CHECK_TRUE(memunit.hasData());

    // Should be full
    CHECK_FALSE(memunit.push_8(0));

    uint8_t data = 0;
    CHECK_TRUE(memunit.pop_8(data));

    CHECK_EQUAL(9, data);

    // Should be okay now
    CHECK_TRUE(memunit.push_8(9));

    for(int i = 9; i >= 0; i--)
    {
        uint8_t data = 0;
        CHECK_TRUE(memunit.pop_8(data));

        CHECK_EQUAL(i, data);
    }

    CHECK_FALSE(memunit.hasData());

    // Should be okay now
    CHECK_TRUE(memunit.push_8(9));
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(MemoryTests, twoBytes)
{
    NABLA::VSYS::Memory<100> memunit;

    CHECK_FALSE(memunit.hasData());

    for(int i = 0; i < 50; i++)
    {
        CHECK_TRUE(memunit.push_16(i));
    }

    // Should be full
    CHECK_FALSE(memunit.push_16(0));

    // With 16, we have to step by 2 to get data
    for(int i = 0; i < 100; i+=2)
    {
        uint16_t data = 0;
        CHECK_TRUE(memunit.get_16(i, data));

        // Data should be i/2 
        CHECK_EQUAL(i/2 , data);

        CHECK_TRUE(memunit.set_16(i, 42));
    }

    for(int i = 99; i > 0; i-=2)
    {
        uint16_t data = 0;
        CHECK_TRUE(memunit.pop_16(data));

        CHECK_EQUAL(42, data);
    }

    CHECK_FALSE(memunit.hasData());

    // Should be okay
    CHECK_TRUE(memunit.push_16(0));
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(MemoryTests, fourBytes)
{
    NABLA::VSYS::Memory<100> memunit;

    for(int i = 0; i < 25; i++)
    {
        CHECK_TRUE(memunit.push_32(i));
    }

    // Should be full
    CHECK_FALSE(memunit.push_32(0));

    for(int i = 0; i < 100; i+= 4)
    {
        uint32_t data = 0;
        CHECK_TRUE(memunit.get_32(i, data));

        CHECK_EQUAL(i/4 , data);

        CHECK_TRUE(memunit.set_32(i, 42));

        data = 0;
        CHECK_TRUE(memunit.get_32(i, data));
    }

    while(memunit.hasData())
    {
        uint32_t data = 0;
        CHECK_TRUE(memunit.pop_32(data));

    }

    CHECK_FALSE(memunit.hasData());

    // Should be okay
    CHECK_TRUE(memunit.push_32(0));
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(MemoryTests, eightBytes)
{
    NABLA::VSYS::Memory<100> memunit;

    for(int i = 0; i < 12; i++)
    {
        CHECK_TRUE(memunit.push_64(i));
    }

    // Should be full
    CHECK_FALSE(memunit.push_64(0));

    // With 64, we have to step by 8 to get data
    for(int i = 0; i < 92; i+=8)
    {
        uint64_t data = 0;
        CHECK_TRUE(memunit.get_64(i, data));

        CHECK_EQUAL(i/8 , data);

        CHECK_TRUE(memunit.set_64(i, 42));

        data = 0;
        CHECK_TRUE(memunit.get_64(i, data));
    }

    while(memunit.hasData())
    {
        uint64_t data = 0;
        CHECK_TRUE(memunit.pop_64(data));

    }

    CHECK_FALSE(memunit.hasData());

    // Should be okay
    CHECK_TRUE(memunit.push_64(0));
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(MemoryTests, mixed)
{
    {
        NABLA::VSYS::Memory<10> memunit;

        CHECK_TRUE(memunit.push_64(4));
        CHECK_TRUE(memunit.push_16(4));
        CHECK_FALSE(memunit.push_8(9));
    }

    {
        NABLA::VSYS::Memory<10> memunit;

        CHECK_TRUE(memunit.push_16(4));
        CHECK_TRUE(memunit.push_16(4));
        CHECK_TRUE(memunit.push_16(4));
        CHECK_TRUE(memunit.push_16(4));
        CHECK_TRUE(memunit.push_16(4));
        CHECK_FALSE(memunit.push_8(9));
    }

    {
        NABLA::VSYS::Memory<16> memunit;

        CHECK_TRUE(memunit.push_64(4));
        CHECK_TRUE(memunit.push_64(4));
        CHECK_FALSE(memunit.push_8(9));
    }

    {
        NABLA::VSYS::Memory<24> memunit;

        CHECK_TRUE(memunit.push_64(1));
        CHECK_TRUE(memunit.push_64(2));

        uint64_t f = 0;
        CHECK_TRUE(memunit.pop_64(f));

        CHECK_EQUAL(f, 2);

        CHECK_TRUE(memunit.push_32(9));
        CHECK_TRUE(memunit.push_32(9));

        CHECK_TRUE(memunit.push_64(2));

        CHECK_FALSE(memunit.push_8(8));
        CHECK_FALSE(memunit.push_16(8));
        CHECK_FALSE(memunit.push_32(8));
        CHECK_FALSE(memunit.push_64(8));
    }

}