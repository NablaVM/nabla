#ifndef NABLA_TYPE_H
#define NABLA_TYPE_H

#include <string.h>
#include <stdint.h>

/*
        Operational promotion rules.
    
        STR > Double > Float > int64 > int32 > int16 > int8 > uint64 > uint32 > uint16 > uint8

        Any operation that contains a type of inequal value, the value is promoted to the 'greater' type.

        Any operation that involves a string, the other operand is promoted to a string.

        If an op contains an int64 and a uint16, the return value will be an int64 from promotion

        Signed integers  >  unsigned as it is assumed the value is signed for a purpose.

        Obviously the could create issues if there is a very large u64 and an int8 is added to it.
            The great way to stop this from happening is by not doing that. 
            This could change if this ends up being silly. 
            To update priority, change the order of the values listed in NablaBaseTypes

*/

enum NablaBaseTypes
{
    NABLA_BASE_TYPE_INTEGER_U_8 ,       // Lowest Priority
    NABLA_BASE_TYPE_INTEGER_U_16,
    NABLA_BASE_TYPE_INTEGER_U_32,
    NABLA_BASE_TYPE_INTEGER_U_64,
    NABLA_BASE_TYPE_INTEGER_S_8 ,
    NABLA_BASE_TYPE_INTEGER_S_16,
    NABLA_BASE_TYPE_INTEGER_S_32,
    NABLA_BASE_TYPE_INTEGER_S_64,
    NABLA_BASE_TYPE_FLOATING_S  ,
    NABLA_BASE_TYPE_FLOATING_D  ,
    NABLA_BASE_TYPE_STRING              // Highest Priority
};

typedef struct BaseType * NablaBaseType;

NablaBaseType basetype_new_uint8 (uint8_t  value);
NablaBaseType basetype_new_uint16(uint16_t value);
NablaBaseType basetype_new_uint32(uint32_t value);
NablaBaseType basetype_new_uint64(uint64_t value);

NablaBaseType basetype_new_int8 (int8_t  value);
NablaBaseType basetype_new_int16(int16_t value);
NablaBaseType basetype_new_int32(int32_t value);
NablaBaseType basetype_new_int64(int64_t value);

NablaBaseType basetype_new_float(float value);
NablaBaseType basetype_new_double(double value);

NablaBaseType basetype_new_str(char* str);

void * basetype_get_data_ptr(NablaBaseType bt);

enum NablaBaseTypes basetype_get_type(NablaBaseType bt);

NablaBaseType basetype_add(NablaBaseType lhs, NablaBaseType rhs, uint8_t * okay);
NablaBaseType basetype_sub(NablaBaseType lhs, NablaBaseType rhs, uint8_t * okay);
NablaBaseType basetype_mul(NablaBaseType lhs, NablaBaseType rhs, uint8_t * okay);
NablaBaseType basetype_div(NablaBaseType lhs, NablaBaseType rhs, uint8_t * okay);

#endif