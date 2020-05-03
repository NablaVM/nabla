#include "../sockets.h"

#include <stddef.h>
#include <stdio.h>

int main(void)
{
     printf("Starting UDP client\n");

    int result = -255;
    struct nabla_socket * this_client = sockets_create_socket(AF_INET, SOCK_DGRAM, 0, NULL, 4098, 0, &result);
    printf("client: sockets_create_socket | result : %i\n", result);

    result = -255;
    struct nabla_socket * remote_server = sockets_create_socket(AF_INET, SOCK_DGRAM, 0, NULL, 4098, 0, &result);
    printf("server: sockets_create_socket | result : %i\n", result);

    char * data = "Hey, this is the UDP client!";

    sockets_connectionless_send(this_client, remote_server, data);

    char response[2048];

    result = -255;
    sockets_connectionless_recv(remote_server, this_client, response, 2047, &result);

    printf("sockets_connectionless_recv | result : %i\n", result);

    if(result > 0)
    {
        response[result] = '\0';
        printf("\nFROM SERVER >> %s\n", response);
    }

    sockets_close(this_client);
    sockets_close(remote_server);

    sockets_delete(this_client);
    sockets_delete(remote_server);
    return 0;
}