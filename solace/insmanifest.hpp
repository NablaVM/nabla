#ifndef SOLACE_INS_MANIFEST_HPP
#define SOLACE_INS_MANIFEST_HPP


#include <iostream>
#include <iomanip>
#include <bitset>

namespace SOLACE
{
    namespace MANIFEST
    {
        constexpr uint8_t INS_ADD             = 0x7C;
        constexpr uint8_t INS_SUB             = 0x80;
        constexpr uint8_t INS_MUL             = 0x84;
        constexpr uint8_t INS_DIV             = 0x88;
        constexpr uint8_t INS_ADDD            = 0x8C;
        constexpr uint8_t INS_SUBD            = 0x90;
        constexpr uint8_t INS_MULD            = 0x94;
        constexpr uint8_t INS_DIVD            = 0x98;

        constexpr uint8_t INS_BGT             = 0x9C;
        constexpr uint8_t INS_BGTE            = 0xA0;  
        constexpr uint8_t INS_BLT             = 0xA4;  
        constexpr uint8_t INS_BLTE            = 0xA8;  
        constexpr uint8_t INS_BEQ             = 0xAC;  
        constexpr uint8_t INS_BNE             = 0xB0;  
        constexpr uint8_t INS_BGTD            = 0xB4;
        constexpr uint8_t INS_BGTED           = 0xB8;
        constexpr uint8_t INS_BLTD            = 0xBC;
        constexpr uint8_t INS_BLTED           = 0xC0;
        constexpr uint8_t INS_BEQD            = 0xC4;
        constexpr uint8_t INS_BNED            = 0xC8; 

        constexpr uint8_t INS_MOV             = 0xCC;
        constexpr uint8_t INS_LDA             = 0xD0;
        constexpr uint8_t INS_LDB             = 0xD4;
        constexpr uint8_t INS_STB             = 0xD8;
        constexpr uint8_t INS_PUSH            = 0xDC;
        constexpr uint8_t INS_POP             = 0xE0;

        constexpr uint8_t INS_JUMP            = 0xE4;
        constexpr uint8_t INS_CALL            = 0xE8;
        constexpr uint8_t INS_RET             = 0xEC;
        constexpr uint8_t INS_EXIT            = 0xF0;
        constexpr uint8_t INS_FUNCTION_CREATE = 0xF4;
        constexpr uint8_t INS_FUNCTION_END    = 0xF8;
        constexpr uint8_t NINS_LABEL          = 0xFC;

        constexpr uint8_t CONST_INT           = 0x70; 
        constexpr uint8_t CONST_DBL           = 0x74;
        constexpr uint8_t CONST_STR           = 0x78;

        // Registers don't have the same 6-bit restrictions
        constexpr uint8_t REGISTER_0          = 0x00;
        constexpr uint8_t REGISTER_1          = 0x01;
        constexpr uint8_t REGISTER_2          = 0x02;
        constexpr uint8_t REGISTER_3          = 0x03;
        constexpr uint8_t REGISTER_4          = 0x04;
        constexpr uint8_t REGISTER_5          = 0x05;
        constexpr uint8_t REGISTER_6          = 0x06;
        constexpr uint8_t REGISTER_7          = 0x07;
        constexpr uint8_t REGISTER_8          = 0x08;
        constexpr uint8_t REGISTER_9          = 0x09;
        constexpr uint8_t REGISTER_10         = 0x0A;
        constexpr uint8_t REGISTER_11         = 0x0B;
        constexpr uint8_t REGISTER_12         = 0x0C;
        constexpr uint8_t REGISTER_13         = 0x0D;
        constexpr uint8_t REGISTER_14         = 0x0E;
        constexpr uint8_t REGISTER_15         = 0x0F;

        void displayManifest()
        {
            std::cout << "INS_ADD             =" << std::hex << (int)INS_ADD             << " \t| " << std::bitset<8>(INS_ADD            ) << std::endl;
            std::cout << "INS_SUB             =" << std::hex << (int)INS_SUB             << " \t| " << std::bitset<8>(INS_SUB            ) << std::endl;
            std::cout << "INS_MUL             =" << std::hex << (int)INS_MUL             << " \t| " << std::bitset<8>(INS_MUL            ) << std::endl;
            std::cout << "INS_DIV             =" << std::hex << (int)INS_DIV             << " \t| " << std::bitset<8>(INS_DIV            ) << std::endl;
            std::cout << "INS_ADDD            =" << std::hex << (int)INS_ADDD            << " \t| " << std::bitset<8>(INS_ADDD           ) << std::endl;
            std::cout << "INS_SUBD            =" << std::hex << (int)INS_SUBD            << " \t| " << std::bitset<8>(INS_SUBD           ) << std::endl;
            std::cout << "INS_MULD            =" << std::hex << (int)INS_MULD            << " \t| " << std::bitset<8>(INS_MULD           ) << std::endl;
            std::cout << "INS_DIVD            =" << std::hex << (int)INS_DIVD            << " \t| " << std::bitset<8>(INS_DIVD           ) << std::endl;
            std::cout << "INS_BGT             =" << std::hex << (int)INS_BGT             << " \t| " << std::bitset<8>(INS_BGT            ) << std::endl;
            std::cout << "INS_BGTE            =" << std::hex << (int)INS_BGTE            << " \t| " << std::bitset<8>(INS_BGTE           ) << std::endl;
            std::cout << "INS_BLT             =" << std::hex << (int)INS_BLT             << " \t| " << std::bitset<8>(INS_BLT            ) << std::endl;
            std::cout << "INS_BLTE            =" << std::hex << (int)INS_BLTE            << " \t| " << std::bitset<8>(INS_BLTE           ) << std::endl;
            std::cout << "INS_BEQ             =" << std::hex << (int)INS_BEQ             << " \t| " << std::bitset<8>(INS_BEQ            ) << std::endl;
            std::cout << "INS_BNE             =" << std::hex << (int)INS_BNE             << " \t| " << std::bitset<8>(INS_BNE            ) << std::endl;
            std::cout << "INS_BGTD            =" << std::hex << (int)INS_BGTD            << " \t| " << std::bitset<8>(INS_BGTD           ) << std::endl;
            std::cout << "INS_BGTED           =" << std::hex << (int)INS_BGTED           << " \t| " << std::bitset<8>(INS_BGTED          ) << std::endl;
            std::cout << "INS_BLTD            =" << std::hex << (int)INS_BLTD            << " \t| " << std::bitset<8>(INS_BLTD           ) << std::endl;
            std::cout << "INS_BLTED           =" << std::hex << (int)INS_BLTED           << " \t| " << std::bitset<8>(INS_BLTED          ) << std::endl;
            std::cout << "INS_BEQD            =" << std::hex << (int)INS_BEQD            << " \t| " << std::bitset<8>(INS_BEQD           ) << std::endl;
            std::cout << "INS_BNED            =" << std::hex << (int)INS_BNED            << " \t| " << std::bitset<8>(INS_BNED           ) << std::endl;
            std::cout << "INS_MOV             =" << std::hex << (int)INS_MOV             << " \t| " << std::bitset<8>(INS_MOV            ) << std::endl;
            std::cout << "INS_LDA             =" << std::hex << (int)INS_LDA             << " \t| " << std::bitset<8>(INS_LDA            ) << std::endl;
            std::cout << "INS_LDB             =" << std::hex << (int)INS_LDB             << " \t| " << std::bitset<8>(INS_LDB            ) << std::endl;
            std::cout << "INS_STB             =" << std::hex << (int)INS_STB             << " \t| " << std::bitset<8>(INS_STB            ) << std::endl;
            std::cout << "INS_PUSH            =" << std::hex << (int)INS_PUSH            << " \t| " << std::bitset<8>(INS_PUSH           ) << std::endl;
            std::cout << "INS_POP             =" << std::hex << (int)INS_POP             << " \t| " << std::bitset<8>(INS_POP            ) << std::endl;
            std::cout << "INS_JUMP            =" << std::hex << (int)INS_JUMP            << " \t| " << std::bitset<8>(INS_JUMP           ) << std::endl;
            std::cout << "INS_CALL            =" << std::hex << (int)INS_CALL            << " \t| " << std::bitset<8>(INS_CALL           ) << std::endl;
            std::cout << "INS_RET             =" << std::hex << (int)INS_RET             << " \t| " << std::bitset<8>(INS_RET            ) << std::endl;
            std::cout << "INS_EXIT            =" << std::hex << (int)INS_EXIT            << " \t| " << std::bitset<8>(INS_EXIT           ) << std::endl;
            std::cout << "INS_FUNCTION_CREATE =" << std::hex << (int)INS_FUNCTION_CREATE << " \t| " << std::bitset<8>(INS_FUNCTION_CREATE) << std::endl;
            std::cout << "INS_FUNCTION_END    =" << std::hex << (int)INS_FUNCTION_END    << " \t| " << std::bitset<8>(INS_FUNCTION_END   ) << std::endl;
            std::cout << "NINS_LABEL          =" << std::hex << (int)NINS_LABEL          << " \t| " << std::bitset<8>(NINS_LABEL         ) << std::endl;
            std::cout << "CONST_INT           =" << std::hex << (int)CONST_INT           << " \t| " << std::bitset<8>(CONST_INT          ) << std::endl;
            std::cout << "CONST_DBL           =" << std::hex << (int)CONST_DBL           << " \t| " << std::bitset<8>(CONST_DBL          ) << std::endl;
            std::cout << "CONST_STR           =" << std::hex << (int)CONST_STR           << " \t| " << std::bitset<8>(CONST_STR          ) << std::endl;
        }


    }
}

#endif

/*
    Possible Instructions   ( lowest 2 bits must be 0 )


    0	| 00000000
    4	| 00000100
    8	| 00001000
    c	| 00001100
    10	| 00010000
    14	| 00010100
    18	| 00011000
    1c	| 00011100
    20	| 00100000
    24	| 00100100
    28	| 00101000
    2c	| 00101100
    30	| 00110000
    34	| 00110100
    38	| 00111000
    3c	| 00111100
    40	| 01000000
    44	| 01000100
    48	| 01001000
    4c	| 01001100
    50	| 01010000
    54	| 01010100
    58	| 01011000
    5c	| 01011100
    60	| 01100000
    64	| 01100100
    68	| 01101000
    6c	| 01101100

    [ BELOW ARE CURRENT INSTRUCTIONS ]
    70	| 01110000
    74	| 01110100
    78	| 01111000
    7c	| 01111100
    80	| 10000000
    84	| 10000100
    88	| 10001000
    8c	| 10001100
    90	| 10010000
    94	| 10010100
    98	| 10011000
    9c	| 10011100
    a0	| 10100000
    a4	| 10100100
    a8	| 10101000
    ac	| 10101100
    b0	| 10110000
    b4	| 10110100
    b8	| 10111000
    bc	| 10111100
    c0	| 11000000
    c4	| 11000100
    c8	| 11001000
    cc	| 11001100
    d0	| 11010000
    d4	| 11010100
    d8	| 11011000
    dc	| 11011100
    e0	| 11100000
    e4	| 11100100
    e8	| 11101000
    ec	| 11101100
    f0	| 11110000
    f4	| 11110100
    f8	| 11111000
    fc	| 11111100



*/