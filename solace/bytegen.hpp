#ifndef SOLACE_BYTEGEN_H
#define SOLACE_BYTEGEN_H

#include <string>
#include <vector>

namespace SOLACE
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
        //! \brief Create a bytegen
        //!
        Bytegen();

        //!  \brief Create a function start
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
        //! \returns Vector of bytes that represent a constant (not ins as constants can be variable length)
        std::vector<uint8_t> createConstantInt(uint32_t val);
        
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

    private:

        uint32_t functionCounter;
    };
}

#endif