#ifndef NABLA_SOCKET_POOL_H
#define NABLA_SOCKET_POOL_H

#include "sockets.h"

#include <stdint.h>

struct sockpool;

typedef struct sockpool sockpool;

sockpool * sockpool_create(uint16_t capacity);

void sockpool_delete(sockpool * sp);

uint16_t sockpool_get_capacity(sockpool * sp);

uint16_t sockpool_get_size(sockpool * sp);

// Allocated a new socket in the pool, and runs sockets_create_socket
// returns index of socket in pool
uint16_t sockpool_create_socket(sockpool * sp,
                                int domain, int type,   int protocol, 
                                char *addr, short port, unsigned setNonBlocking, 
                                int  *result);

// Get socket from the pool by index. If out of range, or non-exist will return NULL
nabla_socket * sockpool_get_socket(sockpool * sp, uint16_t idx);

// Closes and deletes a socket if idx is valid socket
void sockpool_delete_socket(sockpool * sp, uint16_t idx);

#endif