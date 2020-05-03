/*
    This lib is primarily a pass through for socket operations 
    so in the future, it can abstract windows/ linux.

*/

#ifndef NABLA_LIBC_NETWORK_SOCKETS
#define NABLA_LIBC_NETWORK_SOCKETS

#include<sys/socket.h>
#include<arpa/inet.h>

struct nabla_socket
{
    int socket_desc;
    struct sockaddr_in saddr;
};

//! \brief Creates a socket description 
//! \retval -1 Error, return object will be NULL
//! \retval  0 Success.
//! \note Sockets are sockets. Client and server. If client, the address will be fore the remote
//!       While, if its the server, it should be the address to bind to
//!       NULL addr translates to INADDR_ANY
struct nabla_socket * sockets_create_socket(int domain, int type,   int protocol, 
                                            char *addr, short port, unsigned setNonBlocking, 
                                            int  *result);

// Free socket
void sockets_delete(struct nabla_socket* ns);

// Applies to SOCK_STREAM/TCP sockets.
void sockets_connect(struct nabla_socket * ns, int *result);

// Applies to listeners
void sockets_bind(struct nabla_socket *ns, int *result);

// Set the socket into listening mode
void sockets_listen(struct nabla_socket *ns, int backlog, int *result);

// Close the socket
void sockets_close(struct nabla_socket *ns);

// Send data 
void sockets_send(struct nabla_socket * ns, char* data, int *result);

// Recv data
void sockets_recv(struct nabla_socket *ns, char * buffer, unsigned bufferLen, int *result);

// Connectionless send (UDP)
void sockets_connectionless_send(struct nabla_socket * sender, struct nabla_socket * recvr, char* data);

// Connectionless recv (UDP)
void sockets_connectionless_recv(struct nabla_socket * sender, struct nabla_socket * recvr, char *buffer, unsigned bufferLen, int *result);

// Accepts a connection (TCP) If the given socket wasn't set to non-blocking, this call will block
struct nabla_socket * sockets_accept(struct nabla_socket *ns, int *result);

#endif