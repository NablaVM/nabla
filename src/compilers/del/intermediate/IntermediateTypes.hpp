#ifndef DEL_INTERMEDIATE_TYPES_HPP
#define DEL_INTERMEDIATE_TYPES_HPP


#include "CodegenTypes.hpp" // Remove this once assignments are ported to codegen

namespace DEL
{
namespace INTERMEDIATE
{
namespace TYPES
{

    //! \brief Classification of an assignment (what it should result in)
    enum class AssignmentClassifier
    {
        INTEGER, DOUBLE, CHAR //, STRUCT, STRING
    };

    //! \brief A directive given for loading / calling
    enum class DirectiveType
    {
        ID, CALL
    };

    //! \brief Memory position of a directive
    struct DirectiveAllocation
    {
        uint64_t start_pos;
        uint64_t end_pos;
    };

    //! \brief A directive
    struct Directive
    {
        DirectiveType type;
        std::vector<DirectiveAllocation> allocation;
        std::string data;
    };

    //! \brief Parameter information for a function
    struct ParamInfo
    {
        uint64_t start_pos;
        uint64_t end_pos;
        uint16_t param_number;
    };

    //! \brief An instruction / value pair
    struct AssignemntInstruction
    {
        CODEGEN::TYPES::InstructionSet instruction;
        std::string value;          // Primary value for instruction
    };
    
    //! \brief An assignment representation for the codegen to create an assignment
    struct Assignment
    {
        std::string id;
        Memory::MemAlloc memory_info;
        AssignmentClassifier assignment_classifier;
        std::vector<AssignemntInstruction> instructions;
    };
}
}
}


#endif 