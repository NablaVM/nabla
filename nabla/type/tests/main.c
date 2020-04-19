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

int compare_double(double f1, double f2)
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

void check_set()
{
    NablaBaseType bt_u8  = basetype_new_uint8 (8);  assert(  *(uint8_t*)basetype_get_data_ptr(bt_u8 ) == 8  );
    NablaBaseType bt_u16 = basetype_new_uint16(16); assert( *(uint16_t*)basetype_get_data_ptr(bt_u16) == 16 );
    NablaBaseType bt_u32 = basetype_new_uint32(32); assert( *(uint32_t*)basetype_get_data_ptr(bt_u32) == 32 );
    NablaBaseType bt_u64 = basetype_new_uint64(64); assert( *(uint64_t*)basetype_get_data_ptr(bt_u64) == 64 );

    NablaBaseType bt_8  = basetype_new_int8 (-8);   assert(  *(int8_t*)basetype_get_data_ptr(bt_8 ) == -8  );
    NablaBaseType bt_16 = basetype_new_int16(-16);  assert( *(int16_t*)basetype_get_data_ptr(bt_16) == -16 );
    NablaBaseType bt_32 = basetype_new_int32(-32);  assert( *(int32_t*)basetype_get_data_ptr(bt_32) == -32 );
    NablaBaseType bt_64 = basetype_new_int64(-64);  assert( *(int64_t*)basetype_get_data_ptr(bt_64) == -64 );

    NablaBaseType bt_sf = basetype_new_float(42.2);  assert( compare_float( *(float*) basetype_get_data_ptr(bt_sf), 42.2) );
    NablaBaseType bt_df = basetype_new_double(33.3); assert( compare_double(*(double*)basetype_get_data_ptr(bt_df), 33.3) );

    char * testStr = "This is a test string, lets see if this bad boy works!";

    NablaBaseType bt_str = basetype_new_str(testStr);

    assert( 0 == strcmp( (char*)basetype_get_data_ptr(bt_str), testStr) );

   // printf("PRINT: %s\n", (char*)basetype_get_data_ptr(bt_str));
}

void check_adds()
{
    NablaBaseType bt_u8  = basetype_new_uint8 (8); 
    NablaBaseType bt_u16 = basetype_new_uint16(16);
    NablaBaseType bt_u32 = basetype_new_uint32(32);
    NablaBaseType bt_u64 = basetype_new_uint64(64);    

    /*
            Similar types added. Ensures correct value and correct type
    */
    uint8_t okay = 0; 

    NablaBaseType expect_u8_1  = basetype_add(bt_u8, bt_u8, &okay);   
    assert(expect_u8_1);
    assert( *(uint8_t*)basetype_get_data_ptr(expect_u8_1  ) == 16  );
    assert(basetype_get_type(expect_u8_1) == NABLA_BASE_TYPE_INTEGER_U_8 );
    assert(okay); okay = 0;

    NablaBaseType expect_u16_1 = basetype_add(bt_u16, bt_u16, &okay); 
    assert(expect_u16_1);
    assert( *(uint16_t*)basetype_get_data_ptr(expect_u16_1 ) == 32  );
    assert(basetype_get_type(expect_u16_1) == NABLA_BASE_TYPE_INTEGER_U_16 );
    assert(okay); okay = 0;

    NablaBaseType expect_u32_1 = basetype_add(bt_u32, bt_u32, &okay); 
    assert(expect_u32_1);
    assert( *(uint32_t*)basetype_get_data_ptr(expect_u32_1 ) == 64  );
    assert(basetype_get_type(expect_u32_1) == NABLA_BASE_TYPE_INTEGER_U_32 );
    assert(okay); okay = 0;

    NablaBaseType expect_u64_1 = basetype_add(bt_u64, bt_u64, &okay); 
    assert(expect_u64_1);
    assert( *(uint64_t*)basetype_get_data_ptr(expect_u64_1 ) == 128 );
    assert(basetype_get_type(expect_u64_1) == NABLA_BASE_TYPE_INTEGER_U_64 );
    assert(okay); okay = 0;

    NablaBaseType bt_8  = basetype_new_int8 (-8); 
    NablaBaseType bt_16 = basetype_new_int16(-16);
    NablaBaseType bt_32 = basetype_new_int32(-32);
    NablaBaseType bt_64 = basetype_new_int64(-64);

    NablaBaseType expect_8_1  = basetype_add(bt_8, bt_8, &okay);   
    assert(expect_8_1);
    assert( *(int8_t*)basetype_get_data_ptr(expect_8_1  ) == -16  );
    assert(basetype_get_type(expect_8_1) == NABLA_BASE_TYPE_INTEGER_S_8 );

    NablaBaseType expect_16_1 = basetype_add(bt_16, bt_16, &okay); 
    assert(expect_16_1);
    assert( *(int16_t*)basetype_get_data_ptr(expect_16_1 ) == -32  );
    assert(basetype_get_type(expect_16_1) == NABLA_BASE_TYPE_INTEGER_S_16 );

    NablaBaseType expect_32_1 = basetype_add(bt_32, bt_32, &okay); 
    assert(expect_32_1);
    assert( *(int32_t*)basetype_get_data_ptr(expect_32_1 ) == -64  );
    assert(basetype_get_type(expect_32_1) == NABLA_BASE_TYPE_INTEGER_S_32 );

    NablaBaseType expect_64_1 = basetype_add(bt_64, bt_64, &okay); 
    assert(expect_64_1);
    assert( *(int64_t*)basetype_get_data_ptr(expect_64_1 ) == -128 );
    assert(basetype_get_type(expect_64_1) == NABLA_BASE_TYPE_INTEGER_S_64 );


    char * lhs_str = "This is the left, and ";
    char * rhs_str = "this is the right!";
    char * expected_res = "This is the left, and this is the right!";

    NablaBaseType lhs_sbt = basetype_new_str(lhs_str);
    assert(lhs_sbt);

    NablaBaseType rhs_sbt = basetype_new_str(rhs_str);  
    assert(rhs_str);

    NablaBaseType expect_str = basetype_add(lhs_sbt, rhs_sbt, &okay); 
    assert(expect_str);

    assert(0 == strcmp(expected_res, (char*)basetype_get_data_ptr(expect_str)));
}

void check_promotions()
{
    uint8_t okay = 0;   // We don't actually check this here, it is assumed its checked prior to this test
    
    char * testStr = "... STRING ...";
    NablaBaseType bt_u8  = basetype_new_uint8 (8);   
    NablaBaseType bt_u16 = basetype_new_uint16(16);  
    NablaBaseType bt_u32 = basetype_new_uint32(32);  
    NablaBaseType bt_u64 = basetype_new_uint64(64);  
    NablaBaseType bt_8  = basetype_new_int8 (-8);    
    NablaBaseType bt_16 = basetype_new_int16(-16);   
    NablaBaseType bt_32 = basetype_new_int32(-32);   
    NablaBaseType bt_64 = basetype_new_int64(-64);   
    NablaBaseType bt_sf = basetype_new_float(42.2);  
    NablaBaseType bt_df = basetype_new_double(33.3); 
    NablaBaseType bt_ndf = basetype_new_double(-993.3); 
    NablaBaseType bt_str = basetype_new_str(testStr);

    // Only need to do one operation here, as all arithmatic operations
    // call into the same promotion function
    assert(basetype_get_type(basetype_add(bt_u8, bt_u8,   &okay)) == NABLA_BASE_TYPE_INTEGER_U_8);
    assert(basetype_get_type(basetype_add(bt_u8, bt_u16,  &okay)) == NABLA_BASE_TYPE_INTEGER_U_16);
    assert(basetype_get_type(basetype_add(bt_u16, bt_u16, &okay)) == NABLA_BASE_TYPE_INTEGER_U_16);
    assert(basetype_get_type(basetype_add(bt_u8, bt_u32,  &okay)) == NABLA_BASE_TYPE_INTEGER_U_32);
    assert(basetype_get_type(basetype_add(bt_u16, bt_u32, &okay)) == NABLA_BASE_TYPE_INTEGER_U_32);
    assert(basetype_get_type(basetype_add(bt_u32, bt_u32, &okay)) == NABLA_BASE_TYPE_INTEGER_U_32);
    assert(basetype_get_type(basetype_add(bt_u8, bt_u64,  &okay)) == NABLA_BASE_TYPE_INTEGER_U_64);
    assert(basetype_get_type(basetype_add(bt_u16, bt_u64, &okay)) == NABLA_BASE_TYPE_INTEGER_U_64);
    assert(basetype_get_type(basetype_add(bt_u32, bt_u64, &okay)) == NABLA_BASE_TYPE_INTEGER_U_64);
    assert(basetype_get_type(basetype_add(bt_u64, bt_u64, &okay)) == NABLA_BASE_TYPE_INTEGER_U_64);
    assert(basetype_get_type(basetype_add(bt_8, bt_u64,   &okay)) == NABLA_BASE_TYPE_INTEGER_S_8);   
    assert(basetype_get_type(basetype_add(bt_8, bt_8,     &okay)) == NABLA_BASE_TYPE_INTEGER_S_8);
    assert(basetype_get_type(basetype_add(bt_8, bt_16,    &okay)) == NABLA_BASE_TYPE_INTEGER_S_16);
    assert(basetype_get_type(basetype_add(bt_16, bt_32,   &okay)) == NABLA_BASE_TYPE_INTEGER_S_32);
    assert(basetype_get_type(basetype_add(bt_32, bt_64,   &okay)) == NABLA_BASE_TYPE_INTEGER_S_64);
    assert(basetype_get_type(basetype_add(bt_str, bt_u64, &okay)) == NABLA_BASE_TYPE_STRING);    
    assert(basetype_get_type(basetype_add(bt_64, bt_str,  &okay)) == NABLA_BASE_TYPE_STRING);   
    assert(basetype_get_type(basetype_add(bt_df, bt_str,  &okay)) == NABLA_BASE_TYPE_STRING);   
    assert(basetype_get_type(basetype_add(bt_str, bt_df,  &okay)) == NABLA_BASE_TYPE_STRING);  

    printf("Are these okay ? : \n");
    printf("u32:        %s\n", (char*)basetype_get_data_ptr(basetype_add(bt_u32, bt_str, &okay)));
    printf("-32:        %s\n", (char*)basetype_get_data_ptr(basetype_add(bt_32, bt_str,  &okay)));
    printf("-64:        %s\n", (char*)basetype_get_data_ptr(basetype_add(bt_64, bt_str,  &okay)));
    printf("-16:        %s\n", (char*)basetype_get_data_ptr(basetype_add(bt_16, bt_str,  &okay)));
    printf("-8:         %s\n", (char*)basetype_get_data_ptr(basetype_add(bt_8, bt_str,   &okay)));
    printf("-993.300000:%s\n", (char*)basetype_get_data_ptr(basetype_add(bt_ndf, bt_str, &okay)));
    printf("33.300000:  %s\n", (char*)basetype_get_data_ptr(basetype_add(bt_df, bt_str,  &okay)));
    printf("42.200001:  %s\n", (char*)basetype_get_data_ptr(basetype_add(bt_sf, bt_str,  &okay)));
}

int main(void)
{
    check_set();

    check_adds();


    // This one last because we need to ensure everything else is working first
    check_promotions();

    return 0;
}