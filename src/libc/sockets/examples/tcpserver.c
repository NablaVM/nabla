
#include "../sockets.h"

#include <stdio.h>


int main(void)
{
     printf("Starting TCP server\n");

    //
    //      Create an NS socket that points to an ip and port over TCP
    //
    int result = -255;
    struct nabla_socket * ns = sockets_create_socket(AF_INET, SOCK_STREAM, 0, "127.0.0.1", 4096, &result);

    printf("sockets_create_socket | result : %i\n", result);

    //
    //      Bind the created socket
    //
    result = -255;
    sockets_bind(ns, &result);

    printf("sockets_bind | result : %i\n", result);

    //
    //      Set to listen
    //
    result = -155;
    sockets_listen(ns, 10, &result);

    printf("sockets_listen | result : %i\n", result);

    //
    //      Wait for a client
    //
    int doListen = 1;
    while(doListen)
    {
        //
        //      Accept client
        //
        result = -255;
        struct nabla_socket * client = sockets_blocking_accept(ns, &result);

        printf("sockets_blocking_accept | result : %i\n", result);

        if(result == 0)
        {
            //
            //  Get data from client
            //
            char replyBuff[2000];

            result = -255;
            sockets_recv(client, replyBuff, 2000, &result);

            printf("sockets_recv | result : %i\n", result);

            printf("\n DATA FROM CLIENT >> %s \n", replyBuff);

            // 
            //  Respond back
            //
            char * message = "Hello client, I am server\r\n\r\n";

            result = -255;
            sockets_send(client, message, &result);

            printf("sockets_send | result : %i\n", result);

            sockets_close(client);
            sockets_delete(client);
            doListen = 0;
        }
    }
    
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
