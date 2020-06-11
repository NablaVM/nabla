#ifndef DEL_FUNCTION_HPP
#define DEL_FUNCTION_HPP

#include "Types.hpp"
#include "CodegenTypes.hpp"
#include <string>
#include <vector>

#include "CodeBlock.hpp"

namespace DEL
{
namespace CODEGEN
{
    //! \class Function
    //! \brief A representation of a DEL function - Acts to accumulate lines of code for a function, 
    //!        and wrap the user input with supporting code for loading parameters and resizing the GS
    class Function
    {
    public:
        //! \brief Construct a function
        //! \param name The name of the function
        //! \param params Parameter information for function
        Function(std::string name, std::vector<CODEGEN::TYPES::ParamInfo> params);

        //! \brief Destruct the function
        ~Function();

        //! \brief Add a block of code to the function
        //! \param block A pointer to a block of code to add
        //! \post The code from the block will be added to function instructions, and
        //!       The then the function will delete the block 
        void add_block(CODE::Block * block);

        //! \brief Indicate that the building of the function is complete
        //! \returns A vector of assembly code that represents the function and all
        //!          code generated pertaining to the function
        std::vector<std::string>  building_complete();

        //! \brief Build a return
        //! \param return_item Flag to mark the generation of code that will return a value to the caller via r0
        void build_return(bool return_item=true);

        //! \brief Add bytes that will be required by the function
        //! \param num_bytes The number of bytes required
        void add_required_bytes(uint64_t num_bytes);

    private:
        std::string name;                                   //! The name of the function
        std::vector<CODEGEN::TYPES::ParamInfo> params;      //! The parameter information given to the function
        std::vector<std::string> instructions;              //! Instructions within the function
        uint64_t bytes_required;                            //! How many bytes of stack space the function will take up
    
    
    };
}
}


#endif 