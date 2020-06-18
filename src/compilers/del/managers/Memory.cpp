#include "Memory.hpp"

#include <iostream>

namespace DEL
{
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Memory::Memory() : currently_allocated_bytes(0)
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Memory::~Memory()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    bool Memory::is_id_mapped(std::string id)
    {
        if(memory_map.find(id) != memory_map.end())
        {
            return true;
        }
        return false;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Memory::MemAlloc Memory::get_mem_info(std::string id)
    {
        if(memory_map.find(id) != memory_map.end())
        {
            return memory_map[id];
        }

        //std::cout << "MEM : " << id << " not found " << std::endl;

        return MemAlloc{ 0, 0, 0};
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    uint64_t Memory::get_currently_allocated_bytes_amnt() const
    {
        return currently_allocated_bytes;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Memory::reset()
    {
        memory_map.clear();
        currently_allocated_bytes = 0;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Memory::remove_item(std::string id)
    {
        if(is_id_mapped(id))
        {
            memory_map.erase(memory_map.find(id));
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    bool Memory::alloc_mem(std::string id, uint64_t required_size)
    {
        if((currently_allocated_bytes + required_size) > MAX_GLOBAL_MEMORY )
        {
            return false;
        }

        // Safety check during development
        if(required_size % SETTINGS::SYSTEM_WORD_SIZE_BYTES != 0)
        {
            std::cerr << "Memory Manager > Required size for [" << id << "] is does not conform to word boundary" << std::endl;
            exit(EXIT_FAILURE);
        }

        MemAlloc allocated; 

        allocated.bytes_requested = required_size;
        allocated.bytes_alloced   = SETTINGS::SYSTEM_WORD_SIZE_BYTES;   // We only need to actually store address into DS Device

        allocated.start_pos = currently_allocated_bytes;

        currently_allocated_bytes += SETTINGS::SYSTEM_WORD_SIZE_BYTES;

        memory_map[id] = allocated;

        return true;
    }


}