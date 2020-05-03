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
    vm->registers[10] = 0;
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
    printf("process_tcp_in\n");
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