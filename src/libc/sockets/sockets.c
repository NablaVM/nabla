#include "sockets.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

// -------------------------------------------------
//
// -------------------------------------------------

struct nabla_socket * sockets_create_socket(int domain, int type,    int protocol, 
                                            char *addr, short port,  int *result)
{
    assert(addr);
    assert(result);

    struct nabla_socket * ns = (struct nabla_socket*)malloc(sizeof(struct nabla_socket));

    assert(ns);

    ns->socket_desc = socket(domain, type, protocol);

    if(ns->socket_desc == -1)
    {
        *result = -1;
        free(ns);
        return NULL;
    }

    ns->saddr.sin_addr.s_addr = inet_addr(addr);
    ns->saddr.sin_family      = domain;
    ns->saddr.sin_port = htons( port );

    *result = 0;
    return ns;
}

// -------------------------------------------------
//
// -------------------------------------------------

void sockets_delete(struct nabla_socket* ns)
{
    assert(ns);
    free(ns);
    ns = NULL;
}

// -------------------------------------------------
//
// -------------------------------------------------

void sockets_connect(struct nabla_socket * ns, int *result)
{
    assert(ns);
    assert(result);

    if(connect(ns->socket_desc, (struct sockaddr *)&ns->saddr, sizeof(ns->saddr)) < 0)
    {
        *result = -1;
        return;
    }
    *result = 0;
    return;
}

// -------------------------------------------------
//
// -------------------------------------------------

void sockets_send(struct nabla_socket * ns, char* data, int *result)
{
    assert(ns);
    assert(data);
    assert(result);

    if(send(ns->socket_desc, data, strlen(data), 0) < 0)
    {
        *result = -1;
        return;
    }
    *result = 0;
    return;
}

// -------------------------------------------------
//
// -------------------------------------------------

void sockets_recv(struct nabla_socket *ns, char * buffer, unsigned bufferLen, int *result)
{
    assert(ns);
    assert(buffer);
    assert(result);

    if(recv(ns->socket_desc, buffer, bufferLen, 0) < 0)
    {
        *result = -1;
        return;
    }
    *result = 0;
    return;
}

// -------------------------------------------------
//
// -------------------------------------------------

void socket_close(struct nabla_socket *ns)
{
    assert(ns);

    close(ns->socket_desc);
}