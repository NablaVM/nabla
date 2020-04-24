/*
    Josh Bosley

    The actual byte generator. Solace.h/cpp play the part of parsing and ensuring that there aren't any mistakes in the
    hand-written asm code. The bytegen is called into by solace.h/cpp to actually generate the bytes used for the VM.
*/

#ifndef NABLA_BYTEGEN_H
#define NABLA_BYTEGEN_H

#include <string>
#include <vector>

namespace NABLA
{
    //!
    //! \class Bytegen
    //! \brief The bytegen is an innocent creature. It assumes that everything given to it is in order and valid. 
    //!        It is is also cold and uncaring. It has no regard for your sense of order. It does as its told and 
    //!        hands back a result dependant on whatever you put in. Garbage in = Garbage out. 
    //! \note  The bytegen should be used with something like the solace parser, as it ensures that the code is following
    //!        all of the rules.        
    //!
    class Bytegen
    {
    public:

        //!
        //! \brief Bytes representing a single nabla instruction
        //!
        struct Instruction
        {
            uint8_t bytes[8];
        };

        //!
        //! \brief Helper to determine the inputs of arithmatic creation functions
        //!
        enum class ArithmaticSetup
        {
            REG_REG,    //! Two registers
            REG_NUM,    //! A register and a number
            NUM_REG,    //! A number and a register
            NUM_NUM     //! Two numbers
        };

        //!
        //! \brief Helper to determine arithmatic instruction type
        //!
        enum class ArithmaticTypes
        {
            ADD  = 0x00,
            MUL  = 0x01,
            DIV  = 0x02,
            SUB  = 0x03,
            SUBD = 0x04,
            DIVD = 0x05,
            ADDD = 0x06,
            MULD = 0x07
        };

        //!
        //! \brief Types of branches 
        //!
        enum class BranchTypes
        {  
            BGT   = 0x01, 
            BGTE  = 0x02, 
            BLT   = 0x03, 
            BLTE  = 0x04, 
            BEQ   = 0x05, 
            BNE   = 0x06, 
            BGTD  = 0x07,
            BGTED = 0x08,
            BLTD  = 0x09,
            BLTED = 0x0A,
            BEQD  = 0x0B,
            BNED  = 0x0C,
        };

        //!
        //! \brief Types of stacks 
        //!
        enum class Stacks
        {
            GLOBAL,
            LOCAL
        };

        //!
        //! \brief Integer sizes (signed or unsigned)
        //!
        enum class Integers
        {
            EIGHT,
            SIXTEEN,
            THIRTY_TWO,
            SIXTY_FOUR,
        };

        //!
        //! \brief Helper to determine the inputs of mov creation functions
        //!
        enum class MovSetup
        {
            REG_REG,    //! Arg1 is a register, and so is arg 2
            REG_NUM     //! Arg1 is a register, and arg2 is a number
        };

        //!
        //! \brief Types of bitwise operations 
        //!
        enum class BitwiseTypes
        {
            LSH = 0x01,
            RSH = 0x02,
            AND = 0x03,
            OR  = 0x04,
            XOR = 0x05,
            NOT = 0x06
        };

        //!
        //! \brief Create a bytegen
        //!
        Bytegen();

        //!  \brief Get the current function counter (good for call instruction creation)
        //!  \retval Returns the address that will be assigned to the next createFunctionStart.
        uint32_t getCurrentFunctionCouner() const;

        //!  \brief Create a function start - Allows for 2^48 instructions
        //!  \param[in]  name The name of the function - For logging
        //!  \param[in]  numInstructions The number of instructions in function
        //!  \param[out] address The address given to the function
        //!  \returns An instruction representing the creation of a function
        Instruction createFunctionStart(std::string name, uint64_t numInstructions, uint32_t & address);

        //! \brief Create a function END
        //! \returns An instruction representing the end of a function
        Instruction createFunctionEnd();

        //! \brief Create a constant string
        //! \param val The value to create a constant from 
        //! \returns Vector of bytes that represent a constant (not ins as constants can be variable length)
        std::vector<uint8_t> createConstantString(std::string val);
        
        //! \brief Create a constant int
        //! \param val The value to create a constant from 
        //! \param integerType The type of integer to create
        //! \returns Vector of bytes that represent a constant (not ins as constants can be variable length)
        std::vector<uint8_t> createConstantInt(uint64_t val, Integers integerType);
        
        //! \brief Create a constant double 
        //! \param val The value to create a constant from 
        //! \returns Vector of bytes that represent a constant (not ins as constants can be variable length)
        std::vector<uint8_t> createConstantDouble(double val);

        //! \brief Create an arithmatic instruction
        //! \param type The instruction type
        //! \param setup Indicate what arg2 and arg3 mean (num vs registers)
        //! \param arg1  Destination register
        //! \param arg2  Source 1
        //! \param arg3  Source 2
        //! \note  With this function you could attempt to add in-place numerical constants in double-precision instructions.
        //!        _DONT_ try that. You won't like the resul. GARBAGE IN = GARBAGE OUT . RTFM
        Instruction createArithmaticInstruction(ArithmaticTypes type, ArithmaticSetup setup, int16_t arg1, int16_t arg2, int16_t arg3);

        //! \brief Create a branch instruction
        //! \param type The branch type
        //! \param reg1  Register 1
        //! \param reg2  Register 2
        //! \param location Location within function to branch to
        Instruction createBranchInstruction(BranchTypes type, uint8_t reg1, uint8_t reg2, uint32_t location);
        
        //! \brief Create a jump instruction
        //! \param location  Location (adddress) to jump to
        Instruction createJumpInstruction(uint32_t location);

        //! \brief Create a move instruction
        //! \param setup Flag for what argument 2 represents (reg v.s num)
        //! \param reg1  Register 1 (dest)
        //! \param reg2  Register 2 (src)
        Instruction createMovInstruction(MovSetup setup, uint8_t reg1, uint8_t reg2);

        //! \brief Create a push instruction
        //! \param stack The stack to push to
        //! \param reg   The register that contains data to get
        Instruction createPushInstruction(Stacks stack, uint8_t reg);

        //! \brief Create a pop instruction
        //! \param stack The stack to pop from
        //! \param reg   The register to put the data
        Instruction createPopInstruction(Stacks stack, uint8_t reg);

        //! \brief Create a pop instruction
        //! \param stack The stack to put data in
        //! \param location The location in the stack to put the data
        //! \param reg   The register to get the data
        Instruction createStbInstruction(Stacks stack, uint32_t location, uint8_t reg);

        //! \brief Create a pop instruction
        //! \param stack The stack to get data from
        //! \param location The location in the stack to get the data
        //! \param reg   The register to put the data
        Instruction createLdbInstruction(Stacks stack, uint32_t location, uint8_t reg);

        //! \brief Create return instruction
        Instruction createReturnInstruction();

        //! \brief Create a call instruction
        //! \param funcFrom Address of function that is calling
        //! \param ret Where to return to
        //! \param address The address of the function to call
        //! \returns Multiple instructions must be created for calls to occur
        std::vector<Instruction> createCallInstruction(uint32_t funcFrom, uint32_t ret, uint32_t address);

        //! \brief Create exit instruction
        Instruction createExitInstruction();

        //! \brief Create segment const instruction
        std::vector<uint8_t> createSegConstInstruction(uint64_t count);

        //! \brief Create segment func instruction
        std::vector<uint8_t> createSegFuncInstruction(uint64_t entryAddress);

        //! \brief Create segment binary end of file
        std::vector<uint8_t> createSegBinEOF();


        Instruction createBitwiseInstruction(BitwiseTypes type, ArithmaticSetup setup, int16_t arg1, int16_t arg2, int16_t arg3);    

        Instruction createNopInstruction();

    private:

        uint32_t functionCounter;
    };
}

#endif