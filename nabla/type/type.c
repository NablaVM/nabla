#include "type.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

enum NablaBaseTypes
{
    BASE_TYPE_INTEGER_U_8 ,
    BASE_TYPE_INTEGER_U_16,
    BASE_TYPE_INTEGER_U_32,
    BASE_TYPE_INTEGER_U_64,
    BASE_TYPE_INTEGER_S_8 ,
    BASE_TYPE_INTEGER_S_16,
    BASE_TYPE_INTEGER_S_32,
    BASE_TYPE_INTEGER_S_64,
    BASE_TYPE_FLOATING_S  ,
    BASE_TYPE_FLOATING_D  ,
    BASE_TYPE_STRING
};

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

    bt->type = BASE_TYPE_INTEGER_U_8;

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

    bt->type = BASE_TYPE_INTEGER_U_16;

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

    bt->type = BASE_TYPE_INTEGER_U_32;

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

    bt->type = BASE_TYPE_INTEGER_U_64;

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

    bt->type = BASE_TYPE_INTEGER_S_8;

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

    bt->type = BASE_TYPE_INTEGER_S_16;

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

    bt->type = BASE_TYPE_INTEGER_S_32;

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

    bt->type = BASE_TYPE_INTEGER_S_64;

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

    bt->type = BASE_TYPE_FLOATING_S;

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

    bt->type = BASE_TYPE_FLOATING_D;

    return bt;
}

// ------------------------------------------------
//
// ------------------------------------------------

void * basetype_get_raw_value(BT* bt)
{
    assert(bt);
    assert(bt->data);

    return bt->data;
}
