#ifndef NABLA_SOCKET_POOL_H
#define NABLA_SOCKET_POOL_H

#include "sockets.h"

#include <stdint.h>

//! \brief The socket pool structure
struct sockpool;

typedef struct sockpool sockpool;

//! \brief Creates a socket pool
//! \param capacity The total capacity of the pool
//! \returns A pointer to the new socket pool (null if an error in creation)
sockpool * sockpool_create(uint16_t capacity);

//! \brief Delete a socket pool. Closes and deletes all socket
//!        objects in the pool.
//! \post  The socket pool can no longer be used. Its gone
void sockpool_delete(sockpool * sp);

//! \brief Get the capacity of the pool
//! \param sp The socket pool
//! \returns The total capacity of the pool
uint16_t sockpool_get_capacity(sockpool * sp);

//! \brief Get the size of the socket pool
//! \param sp The socket pool
//! \returns The current size of the pool (number of elements)
uint16_t sockpool_get_size(sockpool * sp);

//! \brief Creates a socket in the socket pool. 
//! \param sp The socket pool
//! \param domain The communication domain. Right now only supports AF_INET 
//! \param type   Communication semantics. Supports SOCK_STREAM and SOCK_DGRAM
//! \param protocol The particular protocol 
//! \param addr   The raw address to hand the socket (outbound uses it as destination, inbound uses it as local setup)
//!               A -1 address assigns INADDR_ANY
//! \param port   The port to hand the socket (outbound uses it as destination, inbound uses it as local setup)
//! \param setNonBlocking Assign the nonblocking flag to the socket (0 does nothing, 1 sets the flag)
//! \param result[out] The result of the socket creation [ -1 failure, 0 success]
//! \returns The index of the new socket in the pool 
uint16_t sockpool_create_socket(sockpool * sp,
                                int domain, int type,   int protocol, 
                                int addr, short port, unsigned setNonBlocking, 
                                int  *result);

//! \brief Get socket pointer by index
//! \param sp  The socket pool
//! \param idx The index of the pool to get
//! \returns The nabla_socket pointer from the pool if the index holds a socket.
//!          If the index doesn't hold a socket, it will be NULL
nabla_socket * sockpool_get_socket(sockpool * sp, uint16_t idx);

//! \brief Closes and deletes socket at the given index
//! \param sp  The socket pool
//! \param idx The index of socket to delete
//! \post  The socket at the given index will be closed and deleted if it exists
void sockpool_delete_socket(sockpool * sp, uint16_t idx);

//! \brief Closes socket at the given index
//! \param sp  The socket pool
//! \param idx The index of socket to close
//! \post  The socket at the given index will be closed if it exists
void sockpool_close_socket(sockpool * sp, uint16_t idx);

//! \brief Insert a socket pointer into pool
//! \param sp  The socket pool
//! \param sock The socket
//! \param result The result of insertion [ -1 failure, 0 success]
//! \returns New socket's index in the pool
//! \note If the insertion fails, sock pool will NOT close and delete the socket.
uint16_t sockpool_insert_new(sockpool * sp, nabla_socket * sock, int *result);

#endif