#ifndef DEL_MEMORY_HPP
#define DEL_MEMORY_HPP

#include <map>
#include <string>
#include "SystemSettings.hpp"
#include <libnabla/VSysSettings.hpp>

namespace DEL
{
    // Fwd for friendship! <3
    class SymbolTable;

    //!\class Memory
    //!\brief The memory class determines how many bytes to allocate to something given a type, and tracks relative
    //!       address of something within the current operating function.
    class Memory
    {
    public:
        //! \brief Maximum memory dictated by the VSYS
        static constexpr int MAX_GLOBAL_MEMORY = NABLA::VSYS::NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES;

        //! \brief Create the memory object
        Memory();

        //! \brief Destruct the memory object
        ~Memory();

        //! \brief A return structure for a query about an item's memory information
        struct MemAlloc
        {
            uint64_t bytes_alloced;     //! Bytes allocated
            uint64_t bytes_requested;   //! Bytes requested
            uint64_t start_pos;         //! Start position of element in memory
        };

        //! \brief Check if a given id is mapped
        //! \param id The ID to check if mapped
        //! \retval True if mapped, false otherwise
        bool is_id_mapped(std::string id);

        //! \brief Retrieve memory information of a given object
        //! \retval Memalloc object
        //! \note This method returns {0,0,0} if item not found. Check is_id_mapped() first
        MemAlloc get_mem_info(std::string id);

        //! \brief Retrieve the number of allocated bytes for the current function
        //! \returns Bytes allocated for function
        uint64_t get_currently_allocated_bytes_amnt() const;

        //! \brief Clear the memory map of all contents and reset the position counter
        //! \note This is called by the Analyzer as soon as a function is done generating
        void reset();

        // Give special privs to symbol table
        friend SymbolTable;
        
    private:

        // Allocate some memory. Only the symbol table accesses this
        bool alloc_mem(std::string id, uint64_t minimum_size);

        uint64_t currently_allocated_bytes;
        std::map<std::string, MemAlloc> memory_map;
    };
}

#endif