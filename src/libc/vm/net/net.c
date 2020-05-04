#include "net.h"

#include "sockets.h"
#include "sockpool.h"
#include "util.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define NABLA_NET_DEVICE_SUB_ID_TCP_OUT   0 
#define NABLA_NET_DEVICE_SUB_ID_TCP_IN    1 
#define NABLA_NET_DEVICE_SUB_ID_UDP_OUT  10 
#define NABLA_NET_DEVICE_SUB_ID_UDP_IN   11 
#define NABLA_NET_DEVICE_SUB_ID_SHUTDOWN 50 
#define NABLA_NET_DEVICE_SUB_ID_RESTART  55 

#define NABLA_NET_DEVICE_COMMAND_CREATE   0
#define NABLA_NET_DEVICE_COMMAND_DELETE   1
#define NABLA_NET_DEVICE_COMMAND_CLOSE    2
#define NABLA_NET_DEVICE_COMMAND_POKE     3

#define NABLA_NET_DEVICE_COMMAND_TCP_IN_BIND   10
#define NABLA_NET_DEVICE_COMMAND_TCP_IN_LISTEN 11
#define NABLA_NET_DEVICE_COMMAND_TCP_IN_ACCEPT 12

#define NABLA_NET_DEVICE_COMMAND_TCP_OUT_CONNECT 20
#define NABLA_NET_DEVICE_COMMAND_TCP_OUT_SEND    21
#define NABLA_NET_DEVICE_COMMAND_TCP_OUT_RECEIVE 22

#define NABLA_NET_DEVICE_COMMAND_UDP_IN_BIND    70
#define NABLA_NET_DEVICE_COMMAND_UDP_IN_SEND    71
#define NABLA_NET_DEVICE_COMMAND_UDP_IN_RECEIVE 72

#define NABLA_NET_DEVICE_COMMAND_UDP_OUT_SEND    80
#define NABLA_NET_DEVICE_COMMAND_UDP_OUT_RECEIVE 81

// The network device
struct NETDevice
{
    uint8_t  active;
    sockpool * socket_pool;

};

// A create command 
struct CommandCreate
{
    uint8_t  domain;
    uint8_t  type;
    uint8_t  protocol;
    uint16_t port;
    uint32_t ipAddress;
    uint8_t  blocking;
};

// --------------------------------------------------------------
//
// --------------------------------------------------------------

struct NETDevice * net_new()
{
    struct NETDevice * nd = (struct NETDevice*)malloc(sizeof(struct NETDevice));

    assert(nd);

    nd->socket_pool = sockpool_create(VM_SETTINGS_DEVICE_CONFIG_NET_MAX_CONNECTIONS);

    assert(nd->socket_pool);

    nd->active = 1;

    return nd;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void net_delete(struct NETDevice * nd)
{
    assert(nd);

    // If a shutdown command happens, socket pool could be null
    if(nd->socket_pool != NULL)
    {    
        sockpool_delete(nd->socket_pool);
    }

    free(nd);

    nd = NULL;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_unset_net(struct VM * vm)
{
    // Dont you dare clear register 11 here Mr. Future Josh
    vm->registers[10] = 0;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

struct CommandCreate process_assemble_command_create(struct VM * vm)
{
    struct CommandCreate cc;

    cc.domain   = util_extract_byte(vm->registers[10], 4);
    cc.type     = util_extract_byte(vm->registers[10], 3);
    cc.protocol = util_extract_byte(vm->registers[10], 2);
    cc.port     = util_extract_two_bytes(vm->registers[10], 1);

    cc.ipAddress = (uint32_t)util_extract_two_bytes(vm->registers[11], 7) << 16 | 
                   (uint32_t)util_extract_two_bytes(vm->registers[11], 5);

    cc.blocking = util_extract_byte(vm->registers[11], 3);

//#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf(">>>>>>>>>>> New CommandCreate: \ndomain: %u\ntype: %u\nproto: %u\nport: %u\nAddr: %u\nBlocking: %u\n", 
            cc.domain, cc.type, cc.protocol, cc.port, cc.ipAddress, cc.blocking);
//#endif
    return cc;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_execute_poke_command(struct NETDevice * nd, struct VM * vm)
{
    printf("process_execute_poke_command - Not yet done\n");
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_tcp_out(struct NETDevice * nd, struct VM * vm)
{
    printf("process_tcp_out\n");
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_tcp_in(struct NETDevice * nd, struct VM * vm)
{
//#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("process_tcp_in\n");
//#endif

    uint8_t command = util_extract_byte(vm->registers[10], 5);

    //  --------------------- Check 'shared' commands first ---------------------------
    //
    switch(command)
    {
        case NABLA_NET_DEVICE_COMMAND_CREATE:
        {
            struct CommandCreate cc = process_assemble_command_create(vm);

            if(cc.domain != AF_INET)
            {
                // Right now we need to specify AF_INET, but we don't support anything else
                // so this is an error.
                printf("NETDevice Error : Incorrect domain given for socket create. Currently only supports AF_INET\n");
                return;
            }

            if(cc.type != SOCK_STREAM)
            {
                printf("NETDevice Warning : Expected SOCK_STREAM for TCP IN got : %u . Ignoring command.", cc.type);
                return;
            }

            int okay = -255;
            uint16_t idx = sockpool_create_socket(nd->socket_pool, cc.domain, cc.type, cc.protocol, cc.ipAddress, cc.port, cc.blocking, &okay);

            if(0 != okay)
            {
                // Error creating socket.
                vm->registers[11] = 0;
            }


            printf(">>>>>>>>>>> new object id : %u\n", idx);

            // Place '1' in the result byte of r11 and place idx in the following 2 bytes
            vm->registers[11] = ( (uint64_t)1 << 56 ) | (uint64_t)idx << 40 ;
            return;
        }
        case NABLA_NET_DEVICE_COMMAND_DELETE:
        {
            uint16_t id = util_extract_two_bytes(vm->registers[10], 4);

            sockpool_delete_socket(nd->socket_pool, id);
            return;
        }
        case NABLA_NET_DEVICE_COMMAND_CLOSE :
        {
            uint16_t id = util_extract_two_bytes(vm->registers[10], 4);

            sockpool_close_socket(nd->socket_pool, id);
            return;
        }
        case NABLA_NET_DEVICE_COMMAND_POKE  :
        {
            process_execute_poke_command(nd, vm);
            return;
        }
        default:
            // Just because this happens, doesn't mean much. It just means the command wasn't a 'shared' command
            break;
    }

    // All of the specific commands for TCP IN has an 'id' associated
    //
    uint16_t object_id = util_extract_two_bytes(vm->registers[10], 4);
    
    //  --------------------- Check 'specific' commands second ---------------------------
    //
    switch(command)
    {
        case NABLA_NET_DEVICE_COMMAND_TCP_IN_BIND  :
        {
            nabla_socket * ns = sockpool_get_socket(nd->socket_pool, object_id);

            if(ns == NULL)
            {

                printf(">>>>>>>>>>> socket was null for id : %u\n", object_id);

                // Mark failure, return
                vm->registers[11] = 0;
                return;
            }

            int result = -255;
            sockets_bind(ns, &result);

            if(result < 0)
            {
                
                
                printf(">>>>>>>>>>> Bind fail\n");


                // Mark failure, return
                vm->registers[11] = 0;
                return;
            }

            // Success
            vm->registers[11] = 1;
            return;
        }
        case NABLA_NET_DEVICE_COMMAND_TCP_IN_LISTEN:
        {
            nabla_socket * ns = sockpool_get_socket(nd->socket_pool, object_id);

            if(ns == NULL)
            {
                // Mark failure, return
                vm->registers[11] = 0;
                return;
            }

            uint16_t back_log = util_extract_two_bytes(vm->registers[10], 4);

            int result = -255;
            sockets_listen(ns, back_log, &result);

            if(result < 0)
            {
                // Mark failure, return
                vm->registers[11] = 0;
                return;
            }

            // Success
            vm->registers[11] = 1;
            return;
        }
        case NABLA_NET_DEVICE_COMMAND_TCP_IN_ACCEPT:
        {
            nabla_socket * ns = sockpool_get_socket(nd->socket_pool, object_id);

            if(ns == NULL)
            {
                // Mark failure, return
                vm->registers[11] = 0;
                return;
            }

            int result = -255;
            nabla_socket * new_connection = sockets_accept(ns, &result);

            if(NULL == new_connection || result != 0)
            {
                // Mark failure, return
                vm->registers[11] = 0;
                return;
            }

            result = -255;
            uint16_t new_connection_id = sockpool_insert_new(nd->socket_pool, new_connection, &result);

            // Sock pool doesn't nuke the connection if insert fails
            if(result != 0)
            {
                sockets_close(new_connection);
                sockets_delete(new_connection);
                vm->registers[11] = 0;
                return;
            }

            // Place '1' in the result byte of r11 and place new_connection_id in the following 2 bytes
            vm->registers[11] = ( (uint64_t)1 << 56 ) | (uint64_t)new_connection_id << 40 ;
            return;
        }
        default:
            // We return now because there is nothing left to do. They sent us something dumb
            return;
    }
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_udp_out(struct NETDevice * nd, struct VM * vm)
{
    printf("process_udp_out\n");
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_udp_in(struct NETDevice * nd, struct VM * vm)
{
    printf("process_udp_in\n");
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_shutdown(struct NETDevice * nd)
{
    printf("process_shutdown\n");

    // Process shutdown can only happen when the device is active, so the socket_pool
    // should still be good, and active set to 1. 

    // We'll undo those things and consider it shut down
    nd->active = 0;

    sockpool_delete(nd->socket_pool);

    nd->socket_pool = NULL;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_restart(struct NETDevice * nd)
{
    printf("process_restart\n");
    
    // If the socket pool hasn't been closed out yet (from a shutdown), close it out
    if(nd->socket_pool != NULL)
    {
        // This will ensure that the active flag will be set to 0
        process_shutdown(nd);
    }

    // Re-create the socket pool
    nd->socket_pool = sockpool_create(VM_SETTINGS_DEVICE_CONFIG_NET_MAX_CONNECTIONS);

    // If there was an issue creating the socket pool, the restart failed
    if(nd->socket_pool == NULL)
    {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
        printf("Failed to restart network device\n");
#endif
        return;
    }

    // If the restart was a success, then mark the device as active
    nd->active = 1;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void net_process(struct NETDevice * nd, struct VM * vm)
{
    assert(nd);
    assert(vm);

    uint16_t sub_id = util_extract_byte(vm->registers[10], 6);

    //  If the device has been shutdown the only thing that should be allowed is 
    //  a restart of the device. Anything else will be blocked
    //
    if(nd->active == 0)
    {
        // Check for restart.
        if(sub_id == NABLA_NET_DEVICE_SUB_ID_RESTART)
        {
            process_restart(nd);
            process_unset_net(vm);
            return;
        }
        
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
        printf("Network device has been shut down and is no longer active\n");
#endif
        process_unset_net(vm);
        return;
    }

    //  Making it here means that the device is active. Process the command (or attempt to)
    //
    switch(sub_id)
    {
        case NABLA_NET_DEVICE_SUB_ID_TCP_OUT :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("net_out_tcp\n");
#endif
            process_tcp_out(nd, vm);
            break;
        }
        case NABLA_NET_DEVICE_SUB_ID_TCP_IN  :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("net_in_tcp\n");
#endif
            process_tcp_in(nd, vm);
            break;
        }
        case NABLA_NET_DEVICE_SUB_ID_UDP_OUT :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("net_out_udp\n");
#endif
            process_udp_out(nd, vm);
            break;
        }
        case NABLA_NET_DEVICE_SUB_ID_UDP_IN :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("net_in_udp\n");
#endif
            process_udp_in(nd, vm);
            break;
        }
        case NABLA_NET_DEVICE_SUB_ID_SHUTDOWN:
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("net_shutdown\n");
#endif
            process_shutdown(nd);
            break;
        }

        case NABLA_NET_DEVICE_SUB_ID_RESTART:
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("net_restart\n");
#endif
            process_restart(nd);
            break;
        }

        default:
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("net_process: Unknown sub id\n");
#endif
            return;
    }

    // All processes must finish with an unset
    process_unset_net(vm);
}