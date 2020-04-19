#include "../type.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>

 int compare_float(float f1, float f2)
 {
    float precision = 0.00001;

    if (((f1 - precision) < f2) && 
        ((f1 + precision) > f2))
    {
        return 1;
    }
    else
    {
        return 0;
    }
 }

 int compare_float(double f1, double f2)
 {
    double precision = 0.00001;

    if (((f1 - precision) < f2) && 
        ((f1 + precision) > f2))
    {
        return 1;
    }
    else
    {
        return 0;
    }
 }

int main(void)
{
    NablaBaseType bt_u8  = basetype_new_uint8 (8);  assert(  *(uint8_t*)basetype_get_value(bt_u8 ) == 8  );
    NablaBaseType bt_u16 = basetype_new_uint16(16); assert( *(uint16_t*)basetype_get_value(bt_u16) == 16 );
    NablaBaseType bt_u32 = basetype_new_uint32(32); assert( *(uint32_t*)basetype_get_value(bt_u32) == 32 );
    NablaBaseType bt_u64 = basetype_new_uint64(64); assert( *(uint64_t*)basetype_get_value(bt_u64) == 64 );

    NablaBaseType bt_8  = basetype_new_int8 (-8);   assert(  *(int8_t*)basetype_get_value(bt_8 ) == -8  );
    NablaBaseType bt_16 = basetype_new_int16(-16);  assert( *(int16_t*)basetype_get_value(bt_16) == -16 );
    NablaBaseType bt_32 = basetype_new_int32(-32);  assert( *(int32_t*)basetype_get_value(bt_32) == -32 );
    NablaBaseType bt_64 = basetype_new_int64(-64);  assert( *(int64_t*)basetype_get_value(bt_64) == -64 );

    NablaBaseType bt_sf = basetype_new_float(42.2);  assert( compare_float(*(float*) basetype_get_raw_value(bt_sf), 42.2) );
    NablaBaseType bt_df = basetype_new_double(33.3); assert( compare_double(*(double*)basetype_get_raw_value(bt_df), 33.3) );


    return 0;
}