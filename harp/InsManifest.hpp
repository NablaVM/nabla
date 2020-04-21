#ifndef HARP_INS_MANIFEST_HPP
#define HARP_INS_MANIFEST_HPP

#include <vector>
#include <cstdint>

namespace HARP
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
        constexpr uint8_t GLOBAL_STACK        = 0xFF;
        constexpr uint8_t LOCAL_STACK         = 0xEE;
        constexpr uint8_t INS_CS_SF           = 0x68;
        constexpr uint8_t INS_CS_SR           = 0x6C;
        constexpr uint8_t INS_SEG_CONST       = 0x5C;
        constexpr uint8_t INS_SEG_FUNC        = 0x60;
        constexpr uint8_t INS_SEG_BEOF        = 0x64;

        static std::vector<uint8_t> GetInsVec() {

            return std::vector<uint8_t> { 
                INS_ADD            ,
                INS_SUB            ,
                INS_MUL            ,
                INS_DIV            ,
                INS_ADDD           ,
                INS_SUBD           ,
                INS_MULD           ,
                INS_DIVD           ,
                INS_BGT            ,
                INS_BGTE           ,
                INS_BLT            ,
                INS_BLTE           ,
                INS_BEQ            ,
                INS_BNE            ,
                INS_BGTD           ,
                INS_BGTED          ,
                INS_BLTD           ,
                INS_BLTED          ,
                INS_BEQD           ,
                INS_BNED           ,
                INS_MOV            ,
                INS_LDB            ,
                INS_STB            ,
                INS_PUSH           ,
                INS_POP            ,
                INS_JUMP           ,
                INS_CALL           ,
                INS_RET            ,
                INS_EXIT           ,
                INS_FUNCTION_CREATE,
                INS_FUNCTION_END   ,
                NINS_LABEL         ,
                CONST_INT          ,
                CONST_DBL          ,
                CONST_STR          ,
                REGISTER_0         ,
                REGISTER_1         ,
                REGISTER_2         ,
                REGISTER_3         ,
                REGISTER_4         ,
                REGISTER_5         ,
                REGISTER_6         ,
                REGISTER_7         ,
                REGISTER_8         ,
                REGISTER_9         ,
                REGISTER_10        ,
                REGISTER_11        ,
                REGISTER_12        ,
                REGISTER_13        ,
                REGISTER_14        ,
                REGISTER_15        ,
                GLOBAL_STACK       ,
                LOCAL_STACK        ,
                INS_CS_SF          ,
                INS_CS_SR          ,
                INS_SEG_CONST      ,
                INS_SEG_FUNC       ,
                INS_SEG_BEOF
            };
        }
    }
}

#endif
