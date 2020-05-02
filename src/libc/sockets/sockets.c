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

    if(NULL == addr)
    {
        ns->saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        ns->saddr.sin_addr.s_addr = inet_addr(addr);
    }


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
    close(ns->socket_desc);
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

void sockets_close(struct nabla_socket *ns)
{
    assert(ns);

    close(ns->socket_desc);
}

// -------------------------------------------------
//
// -------------------------------------------------

void sockets_bind(struct nabla_socket *ns, int *result)
{
    assert(ns);
    assert(result);

    if(bind(ns->socket_desc, (struct sockaddr *)&ns->saddr, sizeof(ns->saddr)) < 0)
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

void sockets_listen(struct nabla_socket *ns, int backlog, int *result)
{
    assert(ns);
    assert(result);

    if(listen(ns->socket_desc, backlog) < 0)
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

struct nabla_socket * sockets_blocking_accept(struct nabla_socket *ns, int *result)
{
    assert(ns);
    assert(result);

    struct nabla_socket * remote = (struct nabla_socket*)malloc(sizeof(struct nabla_socket));

    assert(remote);

    int sz = sizeof(struct sockaddr_in);
    remote->socket_desc = accept(ns->socket_desc, (struct sockaddr *)&remote->saddr, (socklen_t*)&sz);

    if(remote->socket_desc < 0)
    {
        free(remote);
        *result = -1;
        return NULL;
    }

    *result = 0;
    return remote;
}

// -------------------------------------------------
//
// -------------------------------------------------

void sockets_connectionless_send(struct nabla_socket * sender, struct nabla_socket * recvr, char* data)
{
    assert(sender);
    assert(recvr);
    assert(data);

    sendto(sender->socket_desc, (const char *)data, strlen(data), 0, (const struct sockaddr *) &recvr->saddr,  sizeof(recvr->saddr)); 
}

// -------------------------------------------------
//
// -------------------------------------------------

void sockets_connectionless_recv(struct nabla_socket * sender, struct nabla_socket * recvr, char *buffer, unsigned bufferLen, int *result)
{
    assert(sender);
    assert(recvr);
    assert(buffer);
    assert(result);
                                                            // might want MSG_DONTWAIT
    *result = recvfrom(recvr->socket_desc, buffer, bufferLen,  MSG_WAITALL , (struct sockaddr *) &sender->saddr, &bufferLen);
}