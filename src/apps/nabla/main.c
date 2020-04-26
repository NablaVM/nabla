#include <stdlib.h>
#include <stdio.h>

#include "vm.h"
#include "binloader.h"

typedef struct VM * NablaVirtualMachine;

//  Load virtual machine
//
int load_vm(FILE * file, NablaVirtualMachine vm)
{
    // Hand the file and vm over to be populated
    switch(bin_load_vm(file, vm))
    {
        case BIN_LOAD_ERROR_NULL_VM:
            perror("The VM given was null");
            return 1;

        case BIN_LOAD_ERROR_FILE_OPEN:    
            perror("There was an error opening the bytecode file"); 
            return 1;

        case BIN_LOAD_ERROR_FAILED_TO_LOAD_CONSTANTS:    
            perror("There was an error loading constants from the bytecode file"); 
            return 1;

        case BIN_LOAD_ERROR_FAILED_TO_LOAD_FUCNTION:
            perror("There was an error loading function from the bytecode file");
            return 1;

        case BIN_LOAD_ERROR_UNHANDLED_INSTRUCTION:   
            perror("The loader came across something it didn't understand and threw a fit"); 
            return 1;

        case BIN_LOAD_ERROR_ALREADY_LOADED:
            perror("The VM has already been loaded");
            return 1;

        case BIN_LOAD_ERROR_EOB_NOT_FOUND:
            perror("Binary file didn't give a binary EOF instruction");
            return 1;

        default:
            return 0;
    }
    return 0;
}

//  Run the loaded virtual machine
//
int run_vm(NablaVirtualMachine vm)
{
    switch(vm_run(vm))
    {       
        case VM_RUN_ERROR_VM_ALREADY_RUNNING:
            perror("VM could not start. It is already running");
            return 1;

        case VM_RUN_ERROR_UNKNOWN_INSTRUCTION:
            perror("VM caught an illegal instruction");
            return 1;

        default:
            return 0;
    }
    return 0;
}

//  Entry
//
int main(int argc, char**argv)
{
    if(argc != 2)
    {
        perror("No input file given");
        exit(EXIT_FAILURE);
    }

    // Create the virutal machine
    NablaVirtualMachine virtualMachine  = vm_new();

    // Ensure the VM is like.. there
    if(virtualMachine == NULL)
    {
        perror("New vm was null\n");
        exit(EXIT_FAILURE);
    }

    // Assume that what the user gave us was a file
    FILE * file_in = fopen(argv[1], "rb");

    if(file_in == NULL)
    {
        printf("Unable to open file %s\n", argv[1]);
        return 1;
    }

    int load_return = load_vm(file_in, virtualMachine);

    fclose(file_in);

    if(0 != load_return)
    {
        exit(EXIT_FAILURE);
    }

    if(0 != run_vm(virtualMachine))
    {
        exit(EXIT_FAILURE);
    }

    return 0;
}