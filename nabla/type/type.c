#include "type.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

struct BaseType
{
    enum NablaBaseTypes type;
    void * data;
};

typedef struct BaseType BT;

// ------------------------------------------------
//
// ------------------------------------------------

BT * basetype_new_uint8(uint8_t value)
{
    BT * bt = (BT*)malloc(sizeof(BT));

    if(NULL == bt) {  free(bt);  return NULL;  }

    bt->data = (void*)malloc(sizeof(uint8_t));

    if(NULL == bt->data) {  free(bt->data); free(bt);  return NULL;  }

    memcpy(bt->data, &value, sizeof(uint8_t));

    bt->type = NABLA_BASE_TYPE_INTEGER_U_8;

    return bt;
}

// ------------------------------------------------
//
// ------------------------------------------------

BT * basetype_new_uint16(uint16_t value)
{
    BT * bt = (BT*)malloc(sizeof(BT));

    if(NULL == bt) {  free(bt);  return NULL;  }

    bt->data = (void*)malloc(sizeof(uint16_t));

    if(NULL == bt->data) {  free(bt->data); free(bt);  return NULL;  }

    memcpy(bt->data, &value, sizeof(uint16_t));

    bt->type = NABLA_BASE_TYPE_INTEGER_U_16;

    return bt;
}

// ------------------------------------------------
//
// ------------------------------------------------

BT * basetype_new_uint32(uint32_t value)
{
    BT * bt = (BT*)malloc(sizeof(BT));

    if(NULL == bt) {  free(bt);  return NULL;  }

    bt->data = (void*)malloc(sizeof(uint32_t));

    if(NULL == bt->data) {  free(bt->data); free(bt);  return NULL;  }

    memcpy(bt->data, &value, sizeof(uint32_t));

    bt->type = NABLA_BASE_TYPE_INTEGER_U_32;

    return bt;
}

// ------------------------------------------------
//
// ------------------------------------------------

BT * basetype_new_uint64(uint64_t value)
{
    BT * bt = (BT*)malloc(sizeof(BT));

    if(NULL == bt) {  free(bt);  return NULL;  }

    bt->data = (void*)malloc(sizeof(uint64_t));

    if(NULL == bt->data) {  free(bt->data); free(bt);  return NULL;  }

    memcpy(bt->data, &value, sizeof(uint64_t));

    bt->type = NABLA_BASE_TYPE_INTEGER_U_64;

    return bt;
}

// ------------------------------------------------
//
// ------------------------------------------------

BT * basetype_new_int8(int8_t value)
{
    BT * bt = (BT*)malloc(sizeof(BT));

    if(NULL == bt) {  free(bt);  return NULL;  }

    bt->data = (void*)malloc(sizeof(int8_t));

    if(NULL == bt->data) {  free(bt->data); free(bt);  return NULL;  }

    memcpy(bt->data, &value, sizeof(int8_t));

    bt->type = NABLA_BASE_TYPE_INTEGER_S_8;

    return bt;
}

// ------------------------------------------------
//
// ------------------------------------------------

BT * basetype_new_int16(int16_t value)
{
    BT * bt = (BT*)malloc(sizeof(BT));

    if(NULL == bt) {  free(bt);  return NULL;  }

    bt->data = (void*)malloc(sizeof(int16_t));

    if(NULL == bt->data) {  free(bt->data); free(bt);  return NULL;  }

    memcpy(bt->data, &value, sizeof(int16_t));

    bt->type = NABLA_BASE_TYPE_INTEGER_S_16;

    return bt;
}

// ------------------------------------------------
//
// ------------------------------------------------

BT * basetype_new_int32(int32_t value)
{
    BT * bt = (BT*)malloc(sizeof(BT));

    if(NULL == bt) {  free(bt);  return NULL;  }

    bt->data = (void*)malloc(sizeof(int32_t));

    if(NULL == bt->data) {  free(bt->data); free(bt);  return NULL;  }

    memcpy(bt->data, &value, sizeof(int32_t));

    bt->type = NABLA_BASE_TYPE_INTEGER_S_32;

    return bt;
}

// ------------------------------------------------
//
// ------------------------------------------------

BT * basetype_new_int64(int64_t value)
{
    BT * bt = (BT*)malloc(sizeof(BT));

    if(NULL == bt) {  free(bt);  return NULL;  }

    bt->data = (void*)malloc(sizeof(int64_t));

    if(NULL == bt->data) {  free(bt->data); free(bt);  return NULL;  }

    memcpy(bt->data, &value, sizeof(int64_t));

    bt->type = NABLA_BASE_TYPE_INTEGER_S_64;

    return bt;
}

// ------------------------------------------------
//
// ------------------------------------------------

BT * basetype_new_float(float value)
{
    BT * bt = (BT*)malloc(sizeof(BT));

    if(NULL == bt) {  free(bt);  return NULL;  }

    bt->data = (void*)malloc(sizeof(float));

    if(NULL == bt->data) {  free(bt->data); free(bt);  return NULL;  }

    memcpy(bt->data, &value, sizeof(float));

    bt->type = NABLA_BASE_TYPE_FLOATING_S;

    return bt;
}

// ------------------------------------------------
//
// ------------------------------------------------

BT * basetype_new_double(double value)
{
    BT * bt = (BT*)malloc(sizeof(BT));

    if(NULL == bt) {  free(bt);  return NULL;  }

    bt->data = (void*)malloc(sizeof(double));

    if(NULL == bt->data) {  free(bt->data); free(bt);  return NULL;  }

    memcpy(bt->data, &value, sizeof(double));

    bt->type = NABLA_BASE_TYPE_FLOATING_D;

    return bt;
}

// ------------------------------------------------
//
// ------------------------------------------------

NablaBaseType basetype_new_str(char* str)
{
    BT * bt = (BT*)malloc(sizeof(BT));

    if(NULL == bt) {  free(bt);  return NULL;  }

    bt->data = (void*)malloc(sizeof(char*) * strlen(str));

    if(NULL == bt->data) {  free(bt->data); free(bt);  return NULL;  }

    strcpy(bt->data, str);

    bt->type = NABLA_BASE_TYPE_STRING;

    return bt;
}

// ------------------------------------------------
//
// ------------------------------------------------

void * basetype_get_data_ptr(BT* bt)
{
    assert(bt);
    assert(bt->data);

    return bt->data;
}

// ------------------------------------------------
//
// ------------------------------------------------

enum NablaBaseTypes basetype_get_type(NablaBaseType bt)
{
    assert(bt);
    return bt->type;
}

// ------------------------------------------------
//  Determine promotion of type 
// ------------------------------------------------

enum NablaBaseTypes determine_promotion_of_nabla_base_type(BT * lhs, BT * rhs)
{
    if(lhs->type > rhs->type)  return lhs->type;
    if(lhs->type <= rhs->type) return rhs->type;
}

// ------------------------------------------------
//
// ------------------------------------------------

BT* basetype_add(BT* lhs, BT* rhs, uint8_t* okay)
{
    assert(lhs); assert(rhs);

    // Determine return type. If they aren't the same type, we need to figure out what the return type will be
    enum NablaBaseTypes returnType = (lhs->type == rhs->type) ? lhs->type : determine_promotion_of_nabla_base_type(lhs, rhs);

    *okay = 1;  // All failures should kill the software, so mark it as okay

    switch(returnType)
    {
        case NABLA_BASE_TYPE_INTEGER_U_8 : 
        {  
            uint8_t  result =  *(uint8_t*)lhs->data +  *(uint8_t*)rhs->data; 
            return basetype_new_uint8 (result);
        } 
        case NABLA_BASE_TYPE_INTEGER_U_16: 
        {  
            uint16_t result = *(uint16_t*)lhs->data + *(uint16_t*)rhs->data; 
            return basetype_new_uint16(result);
        } 
        case NABLA_BASE_TYPE_INTEGER_U_32: 
        {  
            uint32_t result = *(uint32_t*)lhs->data + *(uint32_t*)rhs->data; 
            return basetype_new_uint32(result);
        } 
        case NABLA_BASE_TYPE_INTEGER_U_64: 
        {  
            uint64_t result = *(uint64_t*)lhs->data + *(uint64_t*)rhs->data; 
            return basetype_new_uint64(result);
        } 
        case NABLA_BASE_TYPE_INTEGER_S_8 : 
        {  
            int8_t   result =  *(int8_t*) lhs->data +  *(int8_t*) rhs->data; 
            return basetype_new_int8  (result);
        } 
        case NABLA_BASE_TYPE_INTEGER_S_16: 
        {  
            int16_t  result = *(int16_t*) lhs->data + *(int16_t*) rhs->data; 
            return basetype_new_int16 (result);
        } 
        case NABLA_BASE_TYPE_INTEGER_S_32: 
        {  
            int32_t  result = *(int32_t*) lhs->data + *(int32_t*) rhs->data; 
            return basetype_new_int32 (result);
        } 
        case NABLA_BASE_TYPE_INTEGER_S_64: 
        {  
            int64_t  result = *(int64_t*) lhs->data + *(int64_t*) rhs->data; 
            return basetype_new_int64 (result);
        } 
        case NABLA_BASE_TYPE_FLOATING_S  : 
        {  
            float    result = *(float*)   lhs->data + *(float*)   rhs->data; 
            return basetype_new_float (result);
        } 
        case NABLA_BASE_TYPE_FLOATING_D  : 
        {  
            double   result = *(double*)  lhs->data + *(double*)  rhs->data; 
            return basetype_new_double(result);
        } 
        case NABLA_BASE_TYPE_STRING      : 
        {
            char * lhs_str;
            char * rhs_str;
            char * result_str;
            uint8_t free_lhs = 0;
            uint8_t free_rhs = 0;

            if(lhs->type != NABLA_BASE_TYPE_STRING)
            {
                free_lhs = 1;
                lhs_str = (char*) malloc((100)*sizeof(char));
                assert(lhs_str);

                if(lhs->type >= NABLA_BASE_TYPE_INTEGER_U_8 && 
                   lhs->type <= NABLA_BASE_TYPE_INTEGER_U_64)
                {
                    sprintf(lhs_str, "%lu", *(uint64_t*)lhs->data);
                }
                else if (lhs->type >= NABLA_BASE_TYPE_INTEGER_S_8 &&
                         lhs->type <= NABLA_BASE_TYPE_INTEGER_S_64)
                {
                    if(lhs->type == NABLA_BASE_TYPE_INTEGER_S_8)       snprintf(lhs_str, 64, "%d" , *(int8_t*) lhs->data);
                    else if(lhs->type == NABLA_BASE_TYPE_INTEGER_S_16) snprintf(lhs_str, 64, "%d" , *(int16_t*)lhs->data);
                    else if(lhs->type == NABLA_BASE_TYPE_INTEGER_S_32) snprintf(lhs_str, 64, "%d" , *(int32_t*)lhs->data);
                    else if(lhs->type == NABLA_BASE_TYPE_INTEGER_S_64) snprintf(lhs_str, 64, "%ld", *(int64_t*)lhs->data);
                }
                else if (lhs->type == NABLA_BASE_TYPE_FLOATING_S)
                {
                    sprintf(lhs_str, "%f", *(float*)lhs->data);
                }
                else if (lhs->type == NABLA_BASE_TYPE_FLOATING_D)
                {
                    sprintf(lhs_str, "%f", *(double*)lhs->data);
                }
            }
            else
            {
                lhs_str = (char*)lhs->data;
            }

            if(rhs->type != NABLA_BASE_TYPE_STRING)
            {
                free_rhs = 1;
                rhs_str = (char*) malloc((26)*sizeof(char));
                assert(rhs_str);

                if(rhs->type >= NABLA_BASE_TYPE_INTEGER_U_8 && 
                   rhs->type <= NABLA_BASE_TYPE_INTEGER_U_64)
                {
                    sprintf(rhs_str, "%lu", *(uint64_t*)rhs->data);
                }
                else if (rhs->type >= NABLA_BASE_TYPE_INTEGER_S_8 &&
                         rhs->type <= NABLA_BASE_TYPE_INTEGER_S_64)
                {
                    if(rhs->type == NABLA_BASE_TYPE_INTEGER_S_8)       snprintf(lhs_str, 64, "%d" , *(int8_t*) rhs->data);
                    else if(rhs->type == NABLA_BASE_TYPE_INTEGER_S_16) snprintf(lhs_str, 64, "%d" , *(int16_t*)rhs->data);
                    else if(rhs->type == NABLA_BASE_TYPE_INTEGER_S_32) snprintf(lhs_str, 64, "%d" , *(int32_t*)rhs->data);
                    else if(rhs->type == NABLA_BASE_TYPE_INTEGER_S_64) snprintf(lhs_str, 64, "%ld", *(int64_t*)rhs->data);
                }
                else
                {
                    sprintf(rhs_str, "%f", *(double*)rhs->data);
                }
            }
            else
            {
                rhs_str = (char*)rhs->data;
            }

            size_t lhs_len = strlen(lhs_str);
            size_t rhs_len = strlen(rhs_str);

            result_str = (char*) malloc((lhs_len + rhs_len + 1)*sizeof(char));
            assert(result_str);

            strcat(result_str, lhs_str);
            strcat(result_str, rhs_str);

            BT * ns = basetype_new_str(result_str);

            if(free_lhs) free(lhs_str);
            if(free_rhs) free(rhs_str);

            free(result_str);
            return ns;
        } 
        default: return NULL;
    }

}

// ------------------------------------------------
//
// ------------------------------------------------

BT* basetype_sub(BT* lhs, BT*rhs, uint8_t *okay)
{
    assert(lhs); assert(rhs);

    // Determine return type. If they aren't the same type, we need to figure out what the return type will be
    enum NablaBaseTypes returnType = (lhs->type == rhs->type) ? lhs->type : determine_promotion_of_nabla_base_type(lhs, rhs);

    *okay = 1;
}

// ------------------------------------------------
//
// ------------------------------------------------

BT* basetype_mul(BT* lhs, BT*rhs, uint8_t *okay)
{
    assert(lhs);  assert(rhs);

    // Determine return type. If they aren't the same type, we need to figure out what the return type will be
    enum NablaBaseTypes returnType = (lhs->type == rhs->type) ? lhs->type : determine_promotion_of_nabla_base_type(lhs, rhs);

    *okay = 1;
}

// ------------------------------------------------
//
// ------------------------------------------------

BT* basetype_div(BT* lhs, BT*rhs, uint8_t *okay)
{
    assert(lhs); assert(rhs);

    // Determine return type. If they aren't the same type, we need to figure out what the return type will be
    enum NablaBaseTypes returnType = (lhs->type == rhs->type) ? lhs->type : determine_promotion_of_nabla_base_type(lhs, rhs);

    *okay = 1;
}
