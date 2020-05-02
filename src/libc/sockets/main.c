/*
    This test is meant to help develop the sockets library. Once completed, the network device will leverage the socket
    library to facilitate networking in the virtual machine
*/

#include "sockets.h"

#include <stdio.h>

void tcp_client();

// -------------------------------------------------
//
// -------------------------------------------------

int main(void)
{
    tcp_client();

    return 0;
}

// -------------------------------------------------
//
// -------------------------------------------------

void tcp_client()
{
    printf("Starting TCP client\n");

    //
    //      Create an NS socket that points to an ip and port over TCP
    //
    int result = -255;
    struct nabla_socket * ns = sockets_create_socket(AF_INET, SOCK_STREAM, 0, "140.82.113.3", 80, &result);

    printf("sockets_create_socket | result : %i\n", result);

    //
    //      Connect the created socket
    //
    result = -255;
    sockets_connect(ns, &result);

    printf("sockets_connect | result : %i\n", result);

    //
    //      Send the server an HTTP GET
    //
    char * message = "GET / HTTP/1.1\r\n\r\n";

    result = -255;
    sockets_send(ns, message, &result);

    printf("sockets_send | result : %i\n", result);

    //
    //      Receive a reply from the server
    //
    char replyBuff[2000];

    result = -255;
    sockets_recv(ns, replyBuff, 2000, &result);

    printf("sockets_recv | result : %i\n", result);

    printf("\n >> %s \n", replyBuff);

    //
    //      Delete the socket
    //
    sockets_delete(ns);
}