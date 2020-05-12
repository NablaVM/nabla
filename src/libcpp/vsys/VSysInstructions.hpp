#ifndef NABLA_VSYS_INSTRUCTIONS_HPP
#define NABLA_VSYS_INSTRUCTIONS_HPP

#include <stdint.h>
namespace NABLA
{
namespace VSYS
{

constexpr uint8_t INS_NOP             = 0x00;
constexpr uint8_t INS_SIZE            = 0x04;
constexpr uint8_t INS_LSH             = 0x44;
constexpr uint8_t INS_RSH             = 0x48;
constexpr uint8_t INS_AND             = 0x4c;
constexpr uint8_t INS_OR              = 0x50;
constexpr uint8_t INS_XOR             = 0x54;
constexpr uint8_t INS_NOT             = 0x58;
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
constexpr uint8_t INS_LDW             = 0x38;
constexpr uint8_t INS_STW             = 0x3C;
constexpr uint8_t INS_PUSHW           = 0x30;
constexpr uint8_t INS_POPW            = 0x34;
constexpr uint8_t INS_PUSH            = 0xDC;
constexpr uint8_t INS_POP             = 0xE0;
constexpr uint8_t INS_JUMP            = 0xE4;
constexpr uint8_t INS_CALL            = 0xE8;
constexpr uint8_t INS_RET             = 0xEC;
constexpr uint8_t INS_EXIT            = 0xF0;
constexpr uint8_t INS_YIELD           = 0x40;
constexpr uint8_t NINS_LABEL          = 0xFC;
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
constexpr uint8_t INS_FUNCTION_CREATE = 0xF4;
constexpr uint8_t INS_FUNCTION_END    = 0xF8;
constexpr uint8_t CONST_INT           = 0x70;
constexpr uint8_t CONST_DBL           = 0x74;
constexpr uint8_t CONST_STR           = 0x78;
}
}
#endif 