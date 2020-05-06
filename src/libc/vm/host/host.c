#include "host.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NABLA_HOST_DEVICE_CLOCK           0
#define NABLA_HOST_DEVICE_CLOCKS_PER_SEC  1
#define NABLA_HOST_DEVICE_EPOCH_TIME     10
#define NABLA_HOST_DEVICE_RANDOM         20

struct HOSTDevice
{
    uint64_t hold_rand;     // Random seed
    uint64_t start_clock;   // Clock mark of VM init time
};

// --------------------------------------------------------------
//
// --------------------------------------------------------------

struct HOSTDevice * host_new()
{
    struct HOSTDevice * host = (struct HOSTDevice*)malloc(sizeof(struct HOSTDevice));

    assert(host);

    host->hold_rand = (uint64_t)time(0);

    host->start_clock = (uint64_t)clock();
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void host_mark(struct HOSTDevice * host)
{
    assert(host);

    host->start_clock = (uint64_t)clock();
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void host_delete(struct HOSTDevice * host)
{
    assert(host);

    free(host);
}

// --------------------------------------------------------------
//  Unset activation register
// --------------------------------------------------------------

void process_unset_host(struct VM * vm)
{
    vm->registers[10] = 0;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_clock(struct HOSTDevice * host, struct VM * vm)
{
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("process_clock\n");
#endif

    vm->registers[11] =  (uint64_t)clock() - host->start_clock;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_clock_ps(struct VM * vm)
{
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("process_clock_ps\n");
#endif

    vm->registers[11] = (uint64_t)CLOCKS_PER_SEC;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_epoch(struct VM * vm)
{
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("process_epoch\n");
#endif
    
    time_t seconds;
    seconds = time(NULL);

    vm->registers[11] = (uint64_t)seconds;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_random(struct HOSTDevice * host, struct VM * vm)
{
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("process_random\n");
#endif

    uint64_t rn = (((host->hold_rand = host->hold_rand * 214013L + 2531011L) >> 16) & 0x7fff);

    vm->registers[11] = rn;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void host_process(struct HOSTDevice * host, struct VM * vm)
{
    assert(host);
    assert(vm);

    uint8_t subid = util_extract_byte(vm->registers[10], 6);

    switch(subid)
    {
        case NABLA_HOST_DEVICE_CLOCK:
        {
            process_clock(host, vm);
            break;
        }

        case NABLA_HOST_DEVICE_CLOCKS_PER_SEC:
        {
            process_clock_ps(vm);
            break;
        }

        case NABLA_HOST_DEVICE_EPOCH_TIME:
        {
            process_epoch(vm);
            break;
        }

        case NABLA_HOST_DEVICE_RANDOM:
        {
            process_random(host, vm);
            break;
        }
        
        default:
            vm->registers[11] = 0;
            break;
    }

    process_unset_host(vm);
}