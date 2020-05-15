#ifndef ASSEMBLER_PARSER_H
#define ASSEMBLER_PARSER_H

#include <string>
#include <vector>

namespace ASSEMBLER
{
    //!
    //! \brief Start the parsing of an ASM file. Any included files will be handled internally
    //! \param[in]  asmFile The initial file to start the parse from. This file should contain the 
    //!             entry method for the software.
    //! \param[out] bytes The bytes that the assembler generates. 
    //! \param[in]  verbose Tell the whole story of whats going on 'under the hood'
    //! \retval[true] The compile process was a success, and the bytes vector should contain executable code
    //! \retval[false] An error occured. No bytes should be populated, but if they are, don't use them. They're baddies
    //!
    extern bool ParseAsm(std::string asmFile, std::vector<uint8_t> &bytes, bool verbose=false);
}

#endif