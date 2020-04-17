#include "vm.h"
#include "vmrc.h"
#include "VmInstructions.h"

#include <stdlib.h>
#include <stdint.h>

//  The virtual machine
//
struct VM
{
    uint8_t id;
};

typedef struct VM NVM;

// -----------------------------------------------------
//
// -----------------------------------------------------

NVM * vm_new()
{
    NVM * vm = (NVM*)malloc(sizeof(NVM));

    vm->id = 1;

    return vm;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

int vm_load_file(FILE* file, NVM * vm)
{
    if(vm == NULL) { return VM_ERROR_NULL_VM; }
    if(file == NULL){ return VM_ERROR_FILE_OPEN; }

    // Read in constants - Populate into global stack
    // Once complete, read in functions
    //  If function create found, read all instructions into function frame until function end found
    //  then store that frame into vm
    // Do ^ until all bytes read-in

    uint8_t currentByte;

    enum Load
    {
        START,
        LOAD_CONST,
        LOAD_FUNC,
        DONE
    };

    enum Load loadState = START;

    while (  fread(&currentByte, 1, 1, file)  )  
    { 
        //printf("We just read %hhu\n", currentByte); 

    
        switch(loadState)
        {
            // The top of the file should be all of the constants
            //
            case START:
            {
                // Figure out what constant is being loaded
                //
                uint8_t op = (currentByte & 0xFC);

                if(op == CONST_INT)
                {
                    uint8_t intType = (currentByte & 0x03);

                    // 0 = uint8 | 1 = uint16 | 2 = uint32 | 4 = uint64

                    printf("Constant INT found of type : %hhu\n", intType);
                }
                else if (op == CONST_DBL)
                {
                    // Dbls are all the same size
                    printf("Constant DBL found\n");
                }
                else if (op == CONST_STR)
                {
                    printf("Constant STR found\n");
                    // Next byte is is string size
                }
                break;
            }

            case LOAD_CONST:
            {
                // Load into current const until length reached 
                // of whatever const we were reading
                break;
            }

            case LOAD_FUNC:
            {
                // Start loading a function
                break;
            }

            case DONE:
            {
                // Double check that everything is okay
                break;
            }
        }
    } 

    return 0;
}