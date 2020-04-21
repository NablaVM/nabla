#ifndef HARP_COMPOSER_HPP
#define HARP_COMPOSER_HPP

#include <vector>
#include <cstdint>

/*

    Parse the bytes of a binary and group them for viewing / decompiling

*/

namespace HARP
{
    class Composer
    {
    public:
        struct NablaConstants
        {
            uint64_t leadIns;
            std::vector<uint8_t> definition;
        };

        struct NablaFunction
        {
            uint64_t address;
            std::vector<uint8_t> instructions;
        };

        struct NablaInsGroup
        {
            uint64_t instruction;
            std::vector<uint8_t> correspondingBytes;
        }

        Composer();

        bool populate(std::vector<uint8_t> bytes);

    private:

        std::vector<NablaConstants> constantsDefined;
        std::vector<NablaFunction>  functionsDefined;
        std::vector<NablaInsGroup>  instructionGroupings;

    }
}


#endif