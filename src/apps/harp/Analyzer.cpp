#include "Analyzer.hpp"
#include <iostream>

extern "C" 
{
    #include <stdio.h>
    #include "vm.h"
    #include "binloader.h"
    #include "stack.h"
}

namespace HARP
{
    // --------------------------------------------
    //
    // --------------------------------------------

    Analyzer::Analyzer() : loaded(false)
    {
        // Create the virutal machine
        vm = vm_new();

        // Ensure the VM is like.. there
        if(vm == NULL)
        {
            std::cerr << "Analyzer : New VM was instantiated as NULL" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    Analyzer::~Analyzer()
    {
        std::cout << "Deleting" << std::endl;
        loaded = false;
        vm_delete(vm);
        vm = nullptr;

        // Reset the binloader
        bin_reset();
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    bool Analyzer::loadBin(std::string file)
    {
        /*
                This loads a vm so we can extract the information without having to re-write all of
                the VM load code for no good reason.
        */
        FILE * file_in = fopen(file.c_str(), "rb");

        if(file_in == NULL)
        {
            std::cerr << "Analyzer : Unable to open file " << file << std::endl;
            return false;
        }
        
        // Hand the file and vm over to be populated
        switch(bin_load_vm(file_in, vm))
        {
            case VM_LOAD_ERROR_NULL_VM:
                std::cerr << "Analyzer : The VM given was null" << std::endl;
                return false;

            case VM_LOAD_ERROR_FILE_OPEN:    
                std::cerr << "Analyzer : There was an error opening the bytecode file" << std::endl;
                return false;

            case VM_LOAD_ERROR_FAILED_TO_LOAD_CONSTANTS:    
                std::cerr << "Analyzer : There was an error loading constants from the bytecode file" << std::endl;
                return false;

            case VM_LOAD_ERROR_FAILED_TO_LOAD_FUCNTION:
                std::cerr << "Analyzer : There was an error loading function from the bytecode file" << std::endl;
                return false;

            case VM_LOAD_ERROR_UNHANDLED_INSTRUCTION:   
                std::cerr << "Analyzer : The loader came across something it didn't understand and threw a fit" << std::endl;
                return false;

            case VM_LOAD_ERROR_ALREADY_LOADED:
                std::cerr << "Analyzer : The VM has already been loaded" << std::endl;
                return false;

            case VM_LOAD_ERROR_EOB_NOT_FOUND:
                std::cerr << "Analyzer : Binary file didn't give a binary EOF instruction" << std::endl;
                return false;

            default:
                break;
        }

        // Init the vm
        vm_init(vm);
        
        fclose(file_in);
        loaded = true;
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    std::vector<uint64_t> Analyzer::getGlobalStack()
    {
        std::vector<uint64_t> result;

        if(!loaded)
        {
            return result;
        }

        for(uint64_t i = 0; i < stack_get_size(vm->globalStack); i++)
        {
            int okay = -255;
            uint64_t value = (uint64_t)stack_value_at(i, vm->globalStack, &okay);

            result.push_back(value);
        }

        return result;
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    std::vector<uint64_t> Analyzer::getCallStack()
    {
        std::vector<uint64_t> result;

        if(!loaded)
        {
            return result;
        }

        for(uint64_t i = 0; i < stack_get_size(vm->callStack); i++)
        {
            int okay = -255;
            uint64_t value = (uint64_t)stack_value_at(i, vm->callStack, &okay);

            result.push_back(value);
        }

        return result;
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    std::vector<int64_t> Analyzer::getRegisters()
    {
        std::vector<int64_t> result;

        if(!loaded)
        {
            return result;
        }

        for(int i = 0 ; i < 16; i++)
        {
            result.push_back(vm->registers[i]);
        }

        return result;
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    uint64_t Analyzer::getFunctionPointer()
    {
        return vm->fp;
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    uint64_t Analyzer::getEntryAddress()
    {
        return vm->entryAddress;
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    std::vector<Analyzer::FunctionInfo> Analyzer::getFunctions()
    {
        std::vector<FunctionInfo> functions;

        for(uint64_t i = 0; i < vm->numberOfFunctions; i++)
        {
            FunctionInfo fi;
            fi.address = i;
            
            for(uint64_t j = 0; j < stack_get_size(vm->functions[i].instructions); j++)
            {
                int okay = -255;
                uint64_t value = (uint64_t)stack_value_at(j, vm->functions[i].instructions, &okay);
                fi.instructions.push_back(value);
            }

            functions.push_back(fi);
        }

        return functions;
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    void Analyzer::step(uint64_t n )
    {
        switch(vm_step(vm, n))
        {       
            case VM_RUN_ERROR_VM_ALREADY_RUNNING:
                std::cerr << "VM could not start. It is already running" << std::endl;
                return;

            case VM_RUN_ERROR_UNKNOWN_INSTRUCTION:
                std::cerr << "VM caught an illegal instruction" << std::endl;
                return;

            default:
                return;
        }
    }
}