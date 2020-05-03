#include "sockpool.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

struct sockpool
{
    uint16_t capacity;
    uint16_t size;

    nabla_socket * pool[65535];
};

typedef struct sockpool sockpool;

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

sockpool * sockpool_create(uint16_t capacity)
{
    sockpool * sp = (sockpool*)malloc(sizeof(sockpool));

    if(!sp)
    {
        free(sp);
        return NULL;
    }

    for(uint16_t i = 0; i < 65535; i++)
    {
        sp->pool[i] = NULL;
    }

    sp->capacity = capacity;
    sp->size = 0;

    return sp;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

void sockpool_delete(sockpool * sp)
{
    assert(sp);

    // Close and free all socket objects
    for(uint16_t i = 0; i < sp->capacity; i++)
    {
        sockpool_delete_socket(sp, i);
    }

    free(sp);
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

uint16_t sockpool_get_capacity(sockpool * sp)
{
    assert(sp);
    return sp->capacity;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

uint16_t sockpool_get_size(sockpool * sp)
{
    assert(sp);
    return sp->size;
}

// ---------------------------------------------------------------
//  Find an index that we can assign a new socket to
// ---------------------------------------------------------------

uint16_t internal_locate_open_idx(sockpool * sp, int *okay)
{
    // This could be optimized, but for now this is just fine.

    for(uint16_t i = 0; i < 65535; i++)
    {
        if(sp->pool[i] == NULL)
        {
            *okay = 1;
            return i;
        }
    }

    *okay = 0;
    return 0;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

uint16_t sockpool_create_socket(sockpool * sp,
                                int domain, int type,   int protocol, 
                                char *addr, short port, unsigned setNonBlocking, 
                                int  *result)
{
    assert(sp);

    // Locate an index that we can use for the socket
    int okay = -255;
    uint16_t idx = internal_locate_open_idx(sp, &okay);

    // Make sure we found a spot
    if(okay != 1)
    {
        *result = -1;
        return 0;
    }

    sp->pool[idx] = sockets_create_socket(domain, type, protocol, addr, port, 
                                          setNonBlocking, result);

    if(sp->pool[idx] == NULL)
    {
        *result = -1;
        return 0;
    }

    sp->size++;
    return idx;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

nabla_socket * sockpool_get_socket(sockpool * sp, uint16_t idx)
{
    assert(sp);

    // uint16_t by convention will be in range
    return sp->pool[idx];
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

void sockpool_delete_socket(sockpool * sp, uint16_t idx)
{
    assert(sp);

    if(sp->size == 0)
    {
        return;
    }

    if(sp->pool[idx] == NULL)
    {
        return;
    }

    sockets_close(sp->pool[idx]);

    sockets_delete(sp->pool[idx]);

    sp->pool[idx] = NULL;

    sp->size--;
}