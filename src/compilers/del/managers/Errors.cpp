#include "Errors.hpp"
#include "del_driver.hpp"

namespace DEL
{
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Errors::Errors(Tracker & tracker) : tracker(tracker)
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
        std::cerr << get_error_start(true) << "Symbol \"" << id << "\" already defined" << std::endl;

        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_unknown_id(std::string id, bool is_fatal)
    {
        std::cerr << get_error_start(is_fatal) << "Unknown ID \"" << id << "\"" << std::endl;

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
        std::cerr << get_error_start(true) 
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
        std::cerr << get_error_start(is_fatal) << "[" << from << "]" << error << std::endl; 

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
        std::cerr << get_error_start(is_fatal) << "Type of \"" 
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
        std::cerr << get_error_start(is_fatal) << "Unable to open \"" << name_used << "\" for resulting output!" << std::endl;
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
        std::cerr << get_error_start(true) << "Unknown function call to \"" << name_called << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_mismatched_param_length(std::string caller, std::string callee, uint64_t caller_params, uint64_t callee_params)
    {
        std::cerr << get_error_start(true) << "Function \"" << callee << "\" expects (" << callee_params 
                  << ") parameters, but call from function \"" << caller << "\" gave (" << caller_params << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string Errors::get_error_start(bool is_fatal)
    {
        std::string level = (is_fatal) ? "<FATAL>" : "<WARNING>";

        std::string es = "[Error] " + level + " : ";
        return es;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_calls_return_value_unhandled(std::string caller_function, std::string callee, bool is_fatal)
    {
        std::cerr << get_error_start(is_fatal) << "Function call to \"" << callee << "\" in function \"" << caller_function << "\" has a return value that is not handled" << std::endl;

        if(is_fatal)
        {
            exit(EXIT_FAILURE);
        }  
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string Errors::get_line_no()
    {
        return std::to_string(tracker.get_lines_tracked());
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_no_return(std::string f )
    {
        std::cerr << get_error_start(true) << "Expected 'return <type>' for function :  " << f << std::endl;
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Errors::report_no_main_function()
    {
        std::cerr << get_error_start(true) << "No 'main' method found" << std::endl;
        exit(EXIT_FAILURE);
    }
}