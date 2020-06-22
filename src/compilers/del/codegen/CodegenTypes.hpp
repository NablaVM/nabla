#ifndef DEL_CODEGEN_TYPES_HPP
#define DEL_CODEGEN_TYPES_HPP

#include <string>
#include <vector>
#include <stdint.h>

#include "Memory.hpp"

namespace DEL
{
namespace CODEGEN
{
namespace TYPES
{
    enum class DataClassification
    {
        INTEGER,
        DOUBLE
    };

    enum class LoopType
    {
        FOR,
        WHILE
    };

    //! \brief A set of instructions for the code generator to use in the processing of tokens
    enum class InstructionSet
    {
        // Arithmatic
        ADD, SUB, DIV, 
        MUL, RSH, LSH, 
        
        BW_OR, BW_NOT, BW_XOR, BW_AND,

        // Comparison
        LTE, LT, GTE, GT, EQ, NE, OR, AND, NEGATE, 

        // Built-in
        POW,

        MOD,

        // Load / Store
        LOAD,
        STORE,

        // Data storage allocate
        DS_ALLOC,

        CALL, // Call a function
        MOVE_ADDRESS, // Move data 

        RETURN,

        USE_RAW        // Use the given value (int or str val)
    };

    struct ParamInfo
    {
        uint64_t start_pos;
        uint64_t end_pos;
        uint16_t param_gs_index;
    };

    //
    //  A base instruction type that operations will use directly, and more
    //  more detailed instructions will inherit from 
    //
    class BaseInstruction
    {
    public:
        BaseInstruction(InstructionSet instruction) : instruction(instruction) {}
        virtual ~BaseInstruction() = default;

        InstructionSet instruction;
    };

    //
    //  An instructon whose value could be any of the data classifications
    //
    class RawValueInstruction : public BaseInstruction
    {
    public:
        RawValueInstruction(InstructionSet instruction, uint64_t value, uint64_t byte_len) : 
            BaseInstruction(instruction), value(value), byte_len(byte_len){}

        uint64_t value;
        uint64_t byte_len;
    };

    //
    //  An instructon specifically for a function call
    //
    class CallInstruction : public BaseInstruction
    {
    public:
        CallInstruction(InstructionSet instruction, std::string name, bool expect_return_value = true) : 
            BaseInstruction(instruction), function_name(name), expect_return_value(expect_return_value){}

        std::string function_name;
        bool expect_return_value;
    };

    //
    //  An instruction with a value that represents an address, and a var that represents how many bytes are in that address
    //
    class AddressValueInstruction: public BaseInstruction
    {
    public:
        AddressValueInstruction(InstructionSet instruction, uint64_t value, uint64_t bytes) : 
            BaseInstruction(instruction), value(value), bytes(bytes){}

        uint64_t value;
        uint64_t bytes;
    };

    //
    //  An instruction that instructs movement of data
    //
    class MoveInstruction : public BaseInstruction
    {
    public:
        MoveInstruction(InstructionSet instruction, uint64_t destination, uint64_t source, uint8_t bytes) : 
                BaseInstruction(instruction), destination(destination), source(source), bytes(bytes) {}

        uint64_t destination;       // Location to
        uint64_t source;            // Location from
        uint8_t  bytes;             // Size of the thing to move   
    };

    //
    //  An instruction that instructs allocation of data
    //
    class DSAllocInstruction : public BaseInstruction
    {
    public:
        DSAllocInstruction(InstructionSet instruction, uint64_t bytes_to_alloc) : 
                BaseInstruction(instruction), bytes_to_alloc(bytes_to_alloc) {}

        uint64_t bytes_to_alloc;
    };

    //
    //  A command given to codgen
    //
    class Command
    {
    public:
        std::string id;
        Memory::MemAlloc memory_info;
        DataClassification classification;
        std::vector<BaseInstruction*> instructions;
    };

    //
    //  Information for starting a conditional
    //
    class ConditionalInitiation
    {
    public:
        ConditionalInitiation(Memory::MemAlloc memory_info) : mem_info(memory_info){}

        Memory::MemAlloc mem_info;
    };

    //
    //  Interface for loops
    //
    class LoopIf
    {
    public:
        LoopIf(LoopType type) : type(type){}

        LoopType type;
    };

    //
    //  Information for starting a for loop
    //
    class ForLoopInitiation : public LoopIf
    {
    public:
        ForLoopInitiation(DataClassification classification, 
                       Memory::MemAlloc loop_var,
                       Memory::MemAlloc end_var, 
                       Memory::MemAlloc step) : LoopIf(LoopType::FOR),
        classification(classification), loop_var(loop_var), end_var(end_var), step(step) {}

        DataClassification classification; 
        Memory::MemAlloc loop_var;
        Memory::MemAlloc end_var;
        Memory::MemAlloc step;
    };

    //
    //  Information for starting a while loop
    //
    class WhileInitiation : public LoopIf
    {
    public:
        WhileInitiation(DataClassification classification, Memory::MemAlloc condition) : LoopIf(LoopType::WHILE),
            classification(classification), condition(condition)
            {}

        DataClassification classification;
        Memory::MemAlloc condition;
    };
}
}
}


#endif 