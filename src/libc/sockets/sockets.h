/*
    This lib is primarily a pass through for socket operations 
    so in the future, it can abstract windows/ linux.
*/

#ifndef NABLA_LIBC_NETWORK_SOCKETS
#define NABLA_LIBC_NETWORK_SOCKETS

#include<sys/socket.h>
#include<arpa/inet.h>

//! \brief A struct representing a socket connection
struct nabla_socket;

typedef struct nabla_socket nabla_socket;

//! \brief Creates a nabla socket 
//! \param domain The communication domain. Right now only supports AF_INET 
//! \param type   Communication semantics. Supports SOCK_STREAM and SOCK_DGRAM
//! \param protocol The particular protocol 
//! \param addr   The address to hand the socket (outbound uses it as destination, inbound uses it as local setup)
//!               A NULL address assigns INADDR_ANY
//! \param port   The port to hand the socket (outbound uses it as destination, inbound uses it as local setup)
//! \param setNonBlocking Assign the nonblocking flag to the socket (0 does nothing, 1 sets the flag)
//! \param result[out] The result of the socket creation [ -1 failure, 0 success]
//! \returns Pointer to a new nabla socket is success, otherwise it will be NULL
nabla_socket * sockets_create_socket(int domain, int type,   int protocol, 
                                     char *addr, short port, unsigned setNonBlocking, 
                                     int  *result);

//! \brief Creates a nabla socket 
//! \param domain The communication domain. Right now only supports AF_INET 
//! \param type   Communication semantics. Supports SOCK_STREAM and SOCK_DGRAM
//! \param protocol The particular protocol 
//! \param addr   The integer representation of an address. -1 assigns INADDR_ANY
//! \param port   The port to hand the socket (outbound uses it as destination, inbound uses it as local setup)
//! \param setNonBlocking Assign the nonblocking flag to the socket (0 does nothing, 1 sets the flag)
//! \param result[out] The result of the socket creation [ -1 failure, 0 success]
//! \returns Pointer to a new nabla socket is success, otherwise it will be NULL
nabla_socket * sockets_create_socket_raw_addr(int domain, int type,   int protocol, 
                                              int addr, short port, unsigned setNonBlocking, 
                                              int  *result);

//! \brief Closes and deletes a socket
//! \param ns The network socket pointer
//! \post  The given socket will be closed, freed, and no longer able to be used
//!
void sockets_delete(nabla_socket* ns);

//! \brief Connect a socket to a remote object. This assumes that
//!        that the underlying socket can actually 'connect' i.e is a TCP socket
//! \param ns The network socket pointer
//! \param result[out] The result of the operation [-1 - Failure, 0 - Success]
void sockets_connect(nabla_socket * ns, int *result);

//! \brief Binds a socket. (TCP/UDP)
//! \param ns The network socket pointer
//! \param result[out] The result of the operation [-1 - Failure, 0 - Success]
void sockets_bind(nabla_socket *ns, int *result);

//! \brief Sets a socket in listening mode. (TCP)
//! \param ns The network socket pointer
//! \param backlog Acceptable backlog for connections
//! \param result[out] The result of the operation [-1 - Failure, 0 - Success]
void sockets_listen(nabla_socket *ns, int backlog, int *result);

//! \brief Close the socket
//! \param ns The network socket pointer
void sockets_close(nabla_socket *ns);

//! \brief Connected Send (TCP)
//! \param ns The network socket pointer
//! \param data The data to send
//! \param result[out] The result of the operation [-1 - Failure, 0 - Success]
void sockets_send(nabla_socket * ns, char* data, int *result);

//! \brief Connected Recv (TCP)
//! \param ns The network socket pointer
//! \param buffer Buffer for data
//! \param bufferLen Length of the buffer
//! \param result Amount of data received
void sockets_recv(nabla_socket *ns, char * buffer, unsigned bufferLen, int *result);

//! \brief Conenctionless send (UDP)
//! \param sender The sender socket info
//! \param recvr  The receiver socket info
//! \param data   The data to send
void sockets_connectionless_send(nabla_socket * sender, nabla_socket * recvr, char* data);

//! \brief Conenctionless recv (UDP)
//! \param sender The sender socket info
//! \param recvr  The receiver socket info
//! \param buffer THe buffer to receive to
//! \param bufferLen Length of the beuffer
//! \param result Amount of data received
void sockets_connectionless_recv(nabla_socket * sender, nabla_socket * recvr, char *buffer, unsigned bufferLen, int *result);

// Accepts a connection (TCP) If the given socket wasn't set to non-blocking, this call will block
//! \brief Accept a connection (TCP)
//! \param ns The bound and listening socket
//! \param result The result of the accept [-1 - Failure, 0 - Success]
//! \returns TCP socket of the new connection if result = 0, otherwise it will be NULL
nabla_socket * sockets_accept(nabla_socket *ns, int *result);

#endif