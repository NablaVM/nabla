#include "../sockets.h"

#include <stddef.h>
#include <stdio.h>


int main(void)
{
    printf("Starting UDP server\n");

    int result = -255;
    struct nabla_socket * remote_client = sockets_create_socket(AF_INET, SOCK_DGRAM, 0, NULL, 4098, 0, &result);
    printf("client: sockets_create_socket | result : %i\n", result);

    result = -255;
    struct nabla_socket * this_server = sockets_create_socket(AF_INET, SOCK_DGRAM, 0, NULL, 4098, 0, &result);
    printf("server: sockets_create_socket | result : %i\n", result);

    char * data = "Hey, this is the UDP server!";


    result = -255;
    sockets_bind(this_server, &result);

    printf("sockets_bind | result : %i\n", result);


    int doRecv = 1;
    while(doRecv)
    {
        char response[2048];

        result = -255;
        sockets_connectionless_recv(remote_client, this_server, response, 2047, &result);

        printf("sockets_connectionless_recv | result : %i\n", result);

        if(result > 0)
        {
            response[result] = '\0';

            printf("\nFROM CLIENT >> %s\n", response);
            
            doRecv = 0;

            sockets_connectionless_send(this_server, remote_client, data);
        }
    }


    sockets_close(this_server);
    sockets_close(remote_client);

    sockets_delete(this_server);
    sockets_delete(remote_client);

    return 0;
}