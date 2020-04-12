#ifndef SOLACE_PARSER_H
#define SOLACE_PARSER_H

#include <string>
#include <vector>

namespace SOLACE
{

    extern bool ParseAsm(std::string asmFile, std::vector<uint8_t> &bytes);

}

#endif