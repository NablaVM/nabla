#include "util.h"
#include <assert.h>

// Union for easy conversions between doubles and uint64_ts
union doubleEval
{
    uint64_t val;
    double d;
};

// --------------------------------------------------------------
//
// --------------------------------------------------------------

uint8_t util_extract_byte(uint64_t data, uint8_t idx)
{
    return (data >> (8*idx)) & 0xff;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

uint16_t util_extract_two_bytes(uint64_t data, uint8_t idx)
{
    assert(idx > 0);
    return (data >> (8*(idx-1))) & 0xffff;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

double util_convert_uint64_to_double(uint64_t val)
{
    // Extract from our value
    union doubleEval d; d.val = val;

    // Return double
    return d.d;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

uint64_t util_convert_double_to_uint64(double val)
{
    // Extract from our value
    union doubleEval d; d.d = val;

    // Return uint64_t
    return d.val;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

uint8_t util_check_double_equal(double lhs, double rhs)
{
    double precision = 0.00001;
    if (((lhs - precision) < rhs) && 
        ((lhs + precision) > rhs))
    {
        return 1;
    }
    return 0;
}
