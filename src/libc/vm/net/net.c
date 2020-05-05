#include "net.h"

#include "sockets.h"
#include "sockpool.h"
#include "stack.h"
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

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("New CommandCreate: \ndomain: %u\ntype: %u\nproto: %u\nport: %u\nAddr: %u\nBlocking: %u\n", 
            cc.domain, cc.type, cc.protocol, cc.port, cc.ipAddress, cc.blocking);
#endif
    return cc;
}

// --------------------------------------------------------------
//  This method assumes that all validity checks on range information
//  has taken place and is accurate
// --------------------------------------------------------------

char * process_encode_frame_data(struct VM * vm, uint16_t num_bytes, uint32_t gs_start_addr, uint32_t gs_end_addr)
{
    char * encoded = (char*) malloc(sizeof(char) * num_bytes);

    if(encoded == NULL)
    {
        return NULL;
    }

    // Go through stack - and build the encoded array
    uint64_t encoded_idx = 0;
    for(uint64_t idx = gs_start_addr; idx <= gs_end_addr; idx++)
    {
        int result = -255;
        int64_t frame = stack_value_at(idx, vm->globalStack, &result);

        // Get rid of this assert after testing, just free the encoded array and
        // indicate error in reg11
        assert(result == STACK_OKAY);

        for(int i = 7; i >= 0; i--)
        {
            uint8_t c = frame >> i * 8;

            encoded[encoded_idx] = (char)c;
            encoded_idx++;
        }
    }

    return encoded;
}

// --------------------------------------------------------------
//  This method assumes that all validity checks on range information
//  has taken place and is accurate - [-1 - Failure, 0 - Success]
// --------------------------------------------------------------

int process_decode_frame_from_data(struct VM * vm, char * data, uint16_t num_bytes, uint32_t gs_start_addr, uint32_t gs_end_addr)
{

    return -1;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_execute_poke_command(struct NETDevice * nd, struct VM * vm)
{
    printf("process_execute_poke_command - Not yet done\n");
}

// --------------------------------------------------------------
// Returns 1 if process should end
// --------------------------------------------------------------

uint8_t process_check_for_common_command(struct NETDevice * nd, struct VM * vm, uint8_t caller)
{
    uint8_t command = util_extract_byte(vm->registers[10], 5);

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
                return 1;
            }

            if( caller == NABLA_NET_DEVICE_SUB_ID_TCP_OUT || caller == NABLA_NET_DEVICE_SUB_ID_TCP_IN )
            {
                if(cc.type != SOCK_STREAM)
                {
                    printf("NETDevice Warning : Expected SOCK_STREAM for TCP got : %u . Ignoring command.", cc.type);
                    return 1;
                }
            }
            else
            {
                if(cc.type != SOCK_DGRAM)
                {
                    printf("NETDevice Warning : Expected SOCK_DGRAM for UDP got : %u . Ignoring command.", cc.type);
                    return 1;
                } 
            }

            int okay = -255;
            uint16_t idx = sockpool_create_socket(nd->socket_pool, cc.domain, cc.type, cc.protocol, cc.ipAddress, cc.port, cc.blocking, &okay);

            if(0 != okay)
            {
                // Error creating socket.
                vm->registers[11] = 0;
                return 1;
            }

            // Place '1' in the result byte of r11 and place idx in the following 2 bytes
            vm->registers[11] = ( (uint64_t)1 << 56 ) | (uint64_t)idx << 40 ;
            return 1;
        }
        case NABLA_NET_DEVICE_COMMAND_DELETE:
        {
            uint16_t id = util_extract_two_bytes(vm->registers[10], 4);

            sockpool_delete_socket(nd->socket_pool, id);
            return 1;
        }
        case NABLA_NET_DEVICE_COMMAND_CLOSE :
        {
            uint16_t id = util_extract_two_bytes(vm->registers[10], 4);

            sockpool_close_socket(nd->socket_pool, id);
            return 1;
        }
        case NABLA_NET_DEVICE_COMMAND_POKE  :
        {
            process_execute_poke_command(nd, vm);
            return 1;
        }
        default:
            // Just because this happens, doesn't mean much. It just means the command wasn't a 'shared' command
            break;
    }

    // If we make it here the command wasn't shared and the caller needs to attempt to decode it
    return 0;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_tcp_out(struct NETDevice * nd, struct VM * vm)
{
    printf("process_tcp_out\n");

    //  --------------------- Check 'shared' commands first ---------------------------
    //
    if( 1 ==  process_check_for_common_command(nd, vm, NABLA_NET_DEVICE_SUB_ID_TCP_OUT) )
    {
        return;
    }

    uint8_t command = util_extract_byte(vm->registers[10], 5);
    uint16_t object_id = util_extract_two_bytes(vm->registers[10], 4);

    //  --------------------- Check 'specific' commands second ---------------------------
    //
    switch(command)
    {
        case NABLA_NET_DEVICE_COMMAND_TCP_OUT_CONNECT :
        {
            nabla_socket * ns = sockpool_get_socket(nd->socket_pool, object_id);

            if(ns == NULL)
            {
                // Mark failure, return
                vm->registers[11] = 0;
                return;
            }

            int result = -255;
            sockets_connect(ns, &result);

            if(-1 == result)
            {
                // Mark failure, return
                vm->registers[11] = 0;
                return;
            }

            vm->registers[11] = 1;
            return;
        }
        case NABLA_NET_DEVICE_COMMAND_TCP_OUT_SEND    :
        {
            // Get socket object.
            nabla_socket * ns = sockpool_get_socket(nd->socket_pool, object_id);

            if(ns == NULL)
            {
                vm->registers[11] = 0;
                return;
            }
            
            // Extract information for send
            uint16_t num_bytes = util_extract_two_bytes(vm->registers[10], 2);

            uint32_t gs_start_addr = (uint32_t)util_extract_two_bytes(vm->registers[11], 7) << 16 |
                                     (uint32_t)util_extract_two_bytes(vm->registers[11], 5);
            uint32_t gs_end_addr   = (uint32_t)util_extract_two_bytes(vm->registers[11], 3) << 16 |
                                     (uint32_t)util_extract_two_bytes(vm->registers[11], 1);

            // Error check
            /*
             If NUM BYTES is larger than what could be contained by the start and 
             end address given, the send will be cancelled and an error will be 
             reported in r11.
            */
            if(num_bytes > abs(gs_start_addr - gs_end_addr)* 8)
            {
                vm->registers[11] = 0;
                return;
            }

            char * encoded  = process_encode_frame_data(vm, num_bytes, gs_start_addr, gs_end_addr);

            if(encoded == NULL)
            {
                vm->registers[11] = 0;
                return;
            }

            // Send the data
            int result = -255;
            sockets_send(ns, encoded, &result);

            free(encoded);

            if(result == -1)
            {
                vm->registers[11] = 0;
                return;
            }

            vm->registers[11] = 1;
            return;
        }
        case NABLA_NET_DEVICE_COMMAND_TCP_OUT_RECEIVE :
        {
            // Extract information for recv
            uint16_t num_bytes = util_extract_two_bytes(vm->registers[10], 2);

            uint32_t gs_start_addr = (uint32_t)util_extract_two_bytes(vm->registers[11], 7) << 16 |
                                     (uint32_t)util_extract_two_bytes(vm->registers[11], 5);
            uint32_t gs_end_addr   = (uint32_t)util_extract_two_bytes(vm->registers[11], 3) << 16 |
                                     (uint32_t)util_extract_two_bytes(vm->registers[11], 1);

            // Error check
            /*
             If NUM BYTES is larger than what could be contained by the start and 
             end address given, the send will be cancelled and an error will be 
             reported in r11.
            */
            if(num_bytes > abs(gs_start_addr - gs_end_addr)* 8)
            {
                vm->registers[11] = 0;
                return;
            }

            // Get socket object.
            nabla_socket * ns = sockpool_get_socket(nd->socket_pool, object_id);

            if(ns == NULL)
            {
                vm->registers[11] = 0;
                return;
            }

            // Build a buffer to receive data
            char * recvBuffer = (char*)malloc(sizeof(char) * num_bytes);
            if(recvBuffer == NULL)
            {
                vm->registers[11] = 0;
                return;
            }

            // Attempt recv
            int bytes_received = -255;
            sockets_recv(ns, recvBuffer, num_bytes, &bytes_received);

            // If we didn't get anything indicate it and return
            if(bytes_received <= 0)
            {
                free(recvBuffer);
                vm->registers[11] = 0;
                return;
            }

            if( -1 == process_decode_frame_from_data(vm, recvBuffer, bytes_received, gs_start_addr, gs_end_addr) )
            {
                vm->registers[11] = 0;
            }
            else
            {
                // Bytes received guaranteed to be > 0 if this is reached
                vm->registers[11] = (uint64_t)bytes_received;
            }

            free(recvBuffer);
            return;
        }
        default:
            return;
    }
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_tcp_in(struct NETDevice * nd, struct VM * vm)
{
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("process_tcp_in\n");
#endif

    //  --------------------- Check 'shared' commands first ---------------------------
    //
    if( 1 ==  process_check_for_common_command(nd, vm, NABLA_NET_DEVICE_SUB_ID_TCP_IN) )
    {
        return;
    }

    uint8_t command = util_extract_byte(vm->registers[10], 5);

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
                // Mark failure, return
                vm->registers[11] = 0;
                return;
            }

            int result = -255;
            sockets_bind(ns, &result);

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
    
    //  --------------------- Check 'shared' commands first ---------------------------
    //
    if( 1 ==  process_check_for_common_command(nd, vm, NABLA_NET_DEVICE_SUB_ID_UDP_OUT) )
    {
        return;
    }

    uint8_t command = util_extract_byte(vm->registers[10], 5);

    //  --------------------- Check 'specific' commands second ---------------------------
    //
    switch(command)
    {
        case NABLA_NET_DEVICE_COMMAND_UDP_OUT_SEND   :
        {
            return;
        }
        case NABLA_NET_DEVICE_COMMAND_UDP_OUT_RECEIVE:
        {
            return;
        }
        default:
            return;
    }
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_udp_in(struct NETDevice * nd, struct VM * vm)
{
    printf("process_udp_in\n");
    
    //  --------------------- Check 'shared' commands first ---------------------------
    //
    if( 1 ==  process_check_for_common_command(nd, vm, NABLA_NET_DEVICE_SUB_ID_UDP_IN) )
    {
        return;
    }

    uint8_t command = util_extract_byte(vm->registers[10], 5);

    //  --------------------- Check 'specific' commands second ---------------------------
    //
    switch(command)
    {
        case NABLA_NET_DEVICE_COMMAND_UDP_IN_BIND   :
        {
            return;
        }
        case NABLA_NET_DEVICE_COMMAND_UDP_IN_SEND   :
        {
            return;
        }
        case NABLA_NET_DEVICE_COMMAND_UDP_IN_RECEIVE:
        {
            return;
        }
        default:
            return;
    }
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