#include "Errors.hpp"
#include "del_driver.hpp"

#include <libnabla/termcolor.hpp>

namespace DEL
{
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Errors::Errors(DEL_Driver & driver) : driver(driver)
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Errors::~Errors()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_previously_declared(std::string id)
    {
        display_error_start(true); std::cerr  << "Symbol \"" << id << "\" already defined" << std::endl;

        

        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_unknown_id(std::string id, bool is_fatal)
    {
        display_error_start(is_fatal); std::cerr  << "Unknown ID \"" << id << "\"" << std::endl;

        if(is_fatal)
        {
            exit(EXIT_FAILURE);
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_out_of_memory(std::string symbol, uint64_t size, int max_memory)
    {
        display_error_start(true); std::cerr  
                  << "Allocation of \"" << symbol << "\" (size:" << size 
                  << ") causes mapped memory to exceed target's maximum allowable memory of (" 
                  << max_memory << ") bytes.";

        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_custom(std::string from, std::string error, bool is_fatal)
    {
        display_error_start(is_fatal); std::cerr  << "[" << from << "]" << error << std::endl; 

        if(is_fatal)
        {
            exit(EXIT_FAILURE);
        }        
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_unallowed_type(std::string id, bool is_fatal)
    {
        display_error_start(is_fatal); std::cerr  << "Type of \"" 
                  << id 
                  << "\" Forbids current operation"
                  << std::endl;
        
        if(is_fatal)
        {
            exit(EXIT_FAILURE);
        }        
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_unable_to_open_result_out(std::string name_used, bool is_fatal)
    {
        display_error_start(is_fatal); std::cerr  << "Unable to open \"" << name_used << "\" for resulting output!" << std::endl;
        if(is_fatal)
        {
            exit(EXIT_FAILURE);
        } 
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_callee_doesnt_exist(std::string name_called)
    {
        display_error_start(true); std::cerr  << "Unknown function call to \"" << name_called << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_mismatched_param_length(std::string caller, std::string callee, uint64_t caller_params, uint64_t callee_params)
    {
        display_error_start(true); std::cerr  << "Function \"" << callee << "\" expects (" << callee_params 
                  << ") parameters, but call from function \"" << caller << "\" gave (" << caller_params << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::display_error_start(bool is_fatal)
    {
        std::cerr << "[" << termcolor::red << "Error" << termcolor::reset << "] <";

        if(is_fatal){ std::cerr << termcolor::red    << "FATAL"   << termcolor::reset ;} 
        else        { std::cerr << termcolor::yellow << "WARNING" << termcolor::reset ;} 

        std::cerr << "> (" << termcolor::green  << driver.current_file_from_directive << termcolor::reset << ") : ";
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_calls_return_value_unhandled(std::string caller_function, std::string callee, bool is_fatal)
    {
        display_error_start(is_fatal); std::cerr  << "Function call to \"" << callee << "\" in function \"" << caller_function << "\" has a return value that is not handled" << std::endl;

        if(is_fatal)
        {
            exit(EXIT_FAILURE);
        }  
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_no_return(std::string f )
    {
        display_error_start(true); std::cerr  << "Expected 'return <type>' for function :  " << f << std::endl;
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_no_main_function()
    {
        display_error_start(true); std::cerr  << "No 'main' method found" << std::endl;
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_syntax_error(int line, int column, std::string error_message, std::string line_in_question)
    {
        display_error_start(true); std::cerr  << error_message << std::endl;

        display_line_and_error_pointer(line_in_question, column);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_preproc_file_read_fail(std::vector<std::string> include_crumbs, std::string file_in_question)
    {
        display_error_start(true); std::cerr  << "Unable to open file : " << file_in_question << std::endl;

        if(include_crumbs.size() > 0)
        {
            std::cerr << std::endl << "Include history:" << std::endl;
        }

        for(auto i = include_crumbs.rbegin(); i != include_crumbs.rend(); i++)
        {
            std::cerr << "\t " << (*i) << std::endl;
        }
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_preproc_include_path_not_dir(std::string path)
    {
        display_error_start(true); std::cerr  << "Given include path does not exist : " << path << std::endl;
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_preproc_file_not_found(std::string info, std::string file, std::string from)
    {
        display_error_start(true); std::cerr  << info << " \"" << file << "\" " << "requested by \"" << from  << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::display_line_and_error_pointer(std::string line, int column)
    {
        // Weird case
        if(line.size() == 1)
        {
            std::cerr << termcolor::white << line << termcolor::reset << std::endl;
            std::cerr << termcolor::red << "^" << termcolor::reset << std::endl;
            return;
        }

        int start = (column < 5) ? 0 : column-5;
        int end   = ((uint64_t)(column + 5) > line.size()) ? line.size() : column+5;
        std::string pointer_line;
        for(uint64_t i = 0; i < line.size(); i++)
        {
            if(i == column)
            {
                pointer_line += "^";
            }
            else if(i >= start && i <= end )
            {
                if(i != column)
                {
                    pointer_line += "~";
                }
            }
            else
            {
                pointer_line += " ";
            }
        }
        std::cerr << termcolor::white << line << termcolor::reset << std::endl;
        std::cerr << termcolor::red << pointer_line << termcolor::reset << std::endl;
    }
}