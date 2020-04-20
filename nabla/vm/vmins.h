#ifndef VM_INSTRUCTIONS_H
#define VM_INSTRUCTIONS_H
    #define INS_ADD             0x7C
    #define INS_SUB             0x80
    #define INS_MUL             0x84
    #define INS_DIV             0x88
    #define INS_ADDD            0x8C
    #define INS_SUBD            0x90
    #define INS_MULD            0x94
    #define INS_DIVD            0x98
    #define INS_BGT             0x9C
    #define INS_BGTE            0xA0  
    #define INS_BLT             0xA4  
    #define INS_BLTE            0xA8  
    #define INS_BEQ             0xAC  
    #define INS_BNE             0xB0  
    #define INS_BGTD            0xB4
    #define INS_BGTED           0xB8
    #define INS_BLTD            0xBC
    #define INS_BLTED           0xC0
    #define INS_BEQD            0xC4
    #define INS_BNED            0xC8 
    #define INS_MOV             0xCC
//    #define INS_LDA             0xD0  // No longer in use
    #define INS_LDB             0xD4
    #define INS_STB             0xD8
    #define INS_PUSH            0xDC
    #define INS_POP             0xE0
    #define INS_JUMP            0xE4
    #define INS_CALL            0xE8
    #define INS_RET             0xEC
    #define INS_EXIT            0xF0
    #define NINS_LABEL          0xFC
    #define REGISTER_0          0x00
    #define REGISTER_1          0x01
    #define REGISTER_2          0x02
    #define REGISTER_3          0x03
    #define REGISTER_4          0x04
    #define REGISTER_5          0x05
    #define REGISTER_6          0x06
    #define REGISTER_7          0x07
    #define REGISTER_8          0x08
    #define REGISTER_9          0x09
    #define REGISTER_10         0x0A
    #define REGISTER_11         0x0B
    #define REGISTER_12         0x0C
    #define REGISTER_13         0x0D
    #define REGISTER_14         0x0E
    #define REGISTER_15         0x0F
    #define GLOBAL_STACK        0xFF
    #define LOCAL_STACK         0xEE
    #define INS_CS_SF           0x68
    #define INS_CS_SR           0x6C
    #define INS_SEG_CONST       0x5C
    #define INS_SEG_FUNC        0x60
    #define INS_SEG_BEOF        0x64
    #define INS_FUNCTION_CREATE 0xF4
    #define INS_FUNCTION_END    0xF8
    #define CONST_INT           0x70 
    #define CONST_DBL           0x74
    #define CONST_STR           0x78
#endif