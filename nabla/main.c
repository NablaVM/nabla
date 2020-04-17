#include <stdlib.h>
#include <stdio.h>

#include "vm.h"
#include "vmrc.h"

int main(int argc, char**argv)
{
    if(argc != 2)
    {
        perror("No input file given");
        return 1;
    }

    // Assume that what the user gave us was a file
    FILE * file_in = fopen(argv[1], "r");

    // Create the virutal machine
    NablaVirtualMachine virtualMachine  = vm_new();

    // Ensure the VM is like.. there
    if(virtualMachine == NULL)
    {
        perror("New vm was null\n");
        exit(EXIT_FAILURE);
    }

    // Hand the file and vm over to be populated
    switch(vm_load_file(file_in, virtualMachine))
    {
        case VM_ERROR_NULL_VM:
            perror("The VM given was null");
            exit(EXIT_FAILURE);
            break;

        case VM_ERROR_FILE_OPEN:    
            perror("There was an error opening the bytecode file"); 
            exit(EXIT_FAILURE);
            break;

        default:
            // Everything is okay!
            break;
    }

    return 0;
}