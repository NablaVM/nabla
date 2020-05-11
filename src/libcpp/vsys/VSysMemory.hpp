/*
    VSYS Memory

    Memory has some functions that allow us to 'push, pop, get, and set' 
    uint '8, 16, 32, and 64' values into a uint_8 array. This allows us to work with single bytes and multiple bytes
    in a single list with stack-like calls to support global / local stack operations while making it easy
    for externals to add single bytes wherever they want (instead of pulling a uint64_t and masking the value in)
*/

#ifndef NABLA_VSYS_MEMORY_HPP
#define NABLA_VSYS_MEMORY_HPP

#include "VSysSettings.hpp"

#include <stdint.h>
#include <iterator>

namespace NABLA
{
namespace VSYS
{
    //! \brief A memory structure for the nabla vsys
    template<std::size_t T>
    class Memory
    {
    public:

        //! \brief Construct a memory object
        Memory() : data_back(0) 
        {
            memory = new uint8_t [T];
        }

        //! \brief Destruct a memory object
        ~Memory()
        {
            delete [] memory;
        }

        //! \brief Check if memory contains data placed by system
        //! \returns True if data has been placed, False otherwise
        bool hasData() const
        {
            return (data_back > 0);
        }

        //! \brief Bush data into back of allocated memory
        //! \param data The data to push
        //! \returns True if data could be pushed, False otherwise
        bool push_8(uint8_t data)
        {
            if(data_back >= T)  {  return false;   }

            put_n(data_back, data, 8);

            data_back++;

            return true;
        }

        //! \brief Get data from a specified index
        //! \param[int] idx The index of the data to get
        //! \param[out] data The data gotten
        //! \returns True if data could be gotten, False otherwise
        bool pop_8(uint8_t &data)
        {
            if(data_back == 0) { return false; }

            data_back--;
            data = get_n(data_back, 8);
            return true;
        }
        
        //! \brief Get data from a specified index
        //! \param[int] idx The index of the data to get
        //! \param[out] data The data gotten
        //! \returns True if data could be gotten, False otherwise
        bool get_8(uint64_t idx, uint8_t &data)
        {
            if(idx > T || idx > data_back) { return false; }

            data = get_n(idx, 8);

            return true;
        }

        //! \brief Set data at a specified index
        //! \param[int] idx The index of the data to set
        //! \param[out] data The data set
        //! \returns True if data could be set, False otherwise
        bool set_8(uint64_t idx, uint8_t data)
        {
            if(idx >= T) { return false; }

            put_n(idx, data, 8);

            return true;
        }

        //! \brief Bush data into back of allocated memory
        //! \param data The data to push
        //! \returns True if data could be pushed, False otherwise
        bool push_16(uint16_t data)
        {
            if(data_back+1 >= T) {  return false; }

            put_n(data_back, data, 16);

            data_back += 2;

            return true;
        }

        //! \brief Get data from a specified index
        //! \param[int] idx The index of the data to get
        //! \param[out] data The data gotten
        //! \returns True if data could be gotten, False otherwise
        bool pop_16(uint16_t &data)
        {
            if(data_back-2 < 0 ) { return false; }

            data_back-=2;
            data = get_n(data_back, 16);
            return true;
        }

        //! \brief Get data from a specified index
        //! \param[int] idx The index of the data to get
        //! \param[out] data The data gotten
        //! \returns True if data could be gotten, False otherwise
        bool get_16(uint64_t idx, uint16_t &data)
        {
            if(idx > T || idx > data_back) { return false; }

            data = get_n(idx, 16);

            return true;
        }

        //! \brief Set data at a specified index
        //! \param[int] idx The index of the data to set
        //! \param[out] data The data set
        //! \returns True if data could be set, False otherwise
        bool set_16(uint64_t idx, uint8_t data)
        {
            if(idx+1 >= T) { return false; }

            put_n(idx, data, 16);

            return true;
        }

        //! \brief Bush data into back of allocated memory
        //! \param data The data to push
        //! \returns True if data could be pushed, False otherwise
        bool push_32(uint32_t data)
        {
            if(data_back+3 >= T) {  return false; }

            put_n(data_back, data, 32);

            data_back += 4;

            return true;
        }

        //! \brief Get data from a specified index
        //! \param[int] idx The index of the data to get
        //! \param[out] data The data gotten
        //! \returns True if data could be gotten, False otherwise
        bool pop_32(uint32_t &data)
        {
            if(data_back-4 < 0 ) { return false; }

            data_back-=4;
            data = get_n(data_back, 32);
            return true;
        }

        //! \brief Get data from a specified index
        //! \param[int] idx The index of the data to get
        //! \param[out] data The data gotten
        //! \returns True if data could be gotten, False otherwise
        bool get_32(uint64_t idx, uint32_t &data)
        {
            if(idx > T || idx > data_back) { return false; }

            data = get_n(idx, 32);

            return true;
        }

        //! \brief Set data at a specified index
        //! \param[int] idx The index of the data to set
        //! \param[out] data The data set
        //! \returns True if data could be set, False otherwise
        bool set_32(uint64_t idx, uint8_t data)
        {
            if(idx+3 >= T) { return false; }

            put_n(idx, data, 32);

            return true;
        }

        //! \brief Bush data into back of allocated memory
        //! \param data The data to push
        //! \returns True if data could be pushed, False otherwise
        bool push_64(uint64_t data)
        {
            if(data_back+7 >= T) {  return false; }

            put_n(data_back, data, 64);

            data_back += 8;

            return true;
        }

        //! \brief Get data from a specified index
        //! \param[int] idx The index of the data to get
        //! \param[out] data The data gotten
        //! \returns True if data could be gotten, False otherwise
        bool pop_64(uint64_t &data)
        {
            if(data_back-8 < 0 ) { return false; }

            data_back-=8;
            data = get_n(data_back, 64);
            return true;
        }

        //! \brief Get data from a specified index
        //! \param[int] idx The index of the data to get
        //! \param[out] data The data gotten
        //! \returns True if data could be gotten, False otherwise
        bool get_64(uint64_t idx, uint64_t &data)
        {
            if(idx > T || idx > data_back) { return false; }

            data = get_n(idx, 64);

            return true;
        }

        //! \brief Set data at a specified index
        //! \param[int] idx The index of the data to set
        //! \param[out] data The data set
        //! \returns True if data could be set, False otherwise
        bool set_64(uint64_t idx, uint8_t data)
        {
            if(idx+7 >= T) { return false; }

            put_n(idx, data, 64);

            return true;
        }

    private:

        //  Put any 'n' into memory as long as n % 8 = 0 
        //
        void put_n(uint64_t idx, uint64_t data, uint8_t n)
        {
            for(int i = (n / 8); i > 0; i-- )
            {
                memory[idx++] = (uint8_t)(data >> (8 * (i-1)));
            }
        }

        //  Get any 'n' from memory as long as n % 8 = 0 
        //
        uint64_t get_n(uint64_t idx, uint8_t n)
        {
            uint64_t result = 0;
            for(int i = (n / 8); i > 0; i-- )
            {
                result |= ( (uint64_t)memory[idx++] << (8 * (i-1)));
            }
            return result;
        }

        // The 'back' of stored data, to support push-pop operations
        int64_t data_back;

        uint8_t * memory;
    };
} 
}


#endif