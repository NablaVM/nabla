#include "BinExec.hpp"
#include <iostream>

extern "C" 
{
    #include <stdio.h>
    #include "binloader.h"
    #include "stack.h"
}

namespace NABLA
{
    // --------------------------------------------
    //
    // --------------------------------------------
    
    BinExec::BinExec() : loaded(false)
    {
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    bool BinExec::loadBinaryFromFile(std::string file)
    {
        // Ensure we aren't already loaded 
        if(loaded)
        {
            std::cerr << "Binary already loaded. You can't re-use BinExecs" << std::endl;
            return false; 
        }

        // Create the virutal machine
        vm = vm_new();

        // Ensure the VM is like.. there
        if(vm == NULL)
        {
            std::cerr << "BinExec : New VM was instantiated as NULL" << std::endl;
            return false;
        }

        FILE * file_in = fopen(file.c_str(), "rb");

        if(file_in == NULL)
        {
            std::cerr << "BinExec : Unable to open file " << file << std::endl;
            return false;
        }
        
        // Hand the file and vm over to be populated
        switch(bin_load_vm(file_in, vm))
        {
            case BIN_LOAD_ERROR_NULL_VM:
                std::cerr << "BinExec : The VM given was null" << std::endl;
                return false;

            case BIN_LOAD_ERROR_FILE_OPEN:    
                std::cerr << "BinExec : There was an error opening the bytecode file" << std::endl;
                return false;

            case BIN_LOAD_ERROR_FAILED_TO_LOAD_CONSTANTS:    
                std::cerr << "BinExec : There was an error loading constants from the bytecode file" << std::endl;
                return false;

            case BIN_LOAD_ERROR_FAILED_TO_LOAD_FUCNTION:
                std::cerr << "BinExec : There was an error loading function from the bytecode file" << std::endl;
                return false;

            case BIN_LOAD_ERROR_UNHANDLED_INSTRUCTION:   
                std::cerr << "BinExec : The loader came across something it didn't understand and threw a fit" << std::endl;
                return false;

            case BIN_LOAD_ERROR_ALREADY_LOADED:
                std::cerr << "BinExec : The VM has already been loaded" << std::endl;
                return false;

            case BIN_LOAD_ERROR_EOB_NOT_FOUND:
                std::cerr << "BinExec : Binary file didn't give a binary EOF instruction" << std::endl;
                return false;

            default:
                break;
        }
        
        fclose(file_in);
        loaded = true;
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    bool BinExec::execute()
    {
        switch(vm_run(vm))
        {       
            case VM_RUN_ERROR_VM_ALREADY_RUNNING:
                std::cerr << "VM could not start. It is already running" << std::endl;
                return false;

            case VM_RUN_ERROR_UNKNOWN_INSTRUCTION:
                std::cerr << "VM caught an illegal instruction" << std::endl;
                return false;

            default:
                return true;
        }
        return true;
    }
}
