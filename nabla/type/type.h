#ifndef NABLA_TYPE_H
#define NABLA_TYPE_H

#include <stdint.h>

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

void * basetype_get_value(NablaBaseType bt);

#endif