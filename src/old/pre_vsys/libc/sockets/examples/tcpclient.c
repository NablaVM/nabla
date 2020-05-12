
#include "../sockets.h"

#include <stdio.h>


int main(void)
{
     printf("Starting TCP client\n");

    //
    //      Create an NS socket that points to an ip and port over TCP
    //
    int result = -255;
    struct nabla_socket * ns = sockets_create_socket(AF_INET, SOCK_STREAM, 0, "127.0.0.1", 4096, 0, &result);

    printf("sockets_create_socket | result : %i\n", result);

    //
    //      Connect the created socket
    //
    result = -255;
    sockets_connect(ns, &result);

    printf("sockets_connect | result : %i\n", result);

    //
    //      Send the server a hello
    //
    char * message = "Hello server, I am client\r\n";

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

    replyBuff[result] = '\0';

    printf("\n DATA FROM SERVER >> %s \n", replyBuff);

    //
    //      Close the socket
    //
    sockets_close(ns);

    //
    //      Delete the socket
    //
    sockets_delete(ns);

    return 0;
}
