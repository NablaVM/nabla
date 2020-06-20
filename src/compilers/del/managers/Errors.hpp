#ifndef DEL_ERRORS_HPP
#define DEL_ERRORS_HPP

#include <string>
#include <stdint.h>
#include <vector>

namespace DEL
{
    class DEL_Driver;

    //! \class Errors
    //! \brief This is the primary error handler for the compiler. It has a handful of specific error messages
    //!        that we can tell use to trigger fatals. I would like to extend this for different output levels.
    class Errors
    {
    public:

        //! \brief Create the error object
        Errors(DEL_Driver & driver);

        //! \brief Deconstruct the error object
        ~Errors();

        //! \brief Report that something has already been declared
        //! \param id The thing that has already been declared
        //! \param line_no Line number
        //! \post This method call triggers exit failure
        void report_previously_declared(std::string id, int line_no);

        //! \brief Report an unknown identifier
        //! \param id The id that was discovered and unknown
        //! \param line_no Line number
        //! \param is_fatal Triggers exit if true
        void report_unknown_id(std::string id, int line_no, bool is_fatal);

        //! \brief Report an unallowed type
        //! \param is The id that was being set to something incorrect
        //! \param line_no Line number
        //! \param is_fatal Triggers exit if true
        void report_unallowed_type(std::string id, int line_no, bool is_fatal);

        //! \brief Report that the system has attempted to allocate a stack greater than allowed by VSys
        //! \param symbol The last symbol allocated that triggered the issue
        //! \param size The size of the object allocated last
        //! \param max_memory The maximum amount of memory the current system allows
        void report_out_of_memory(std::string symbol, uint64_t size, int max_memory);

        //! \brief Report unable to open a file for a result output
        //! \param name_used The name of the file
        //! \param is_fatal Triggers exit if true
        void report_unable_to_open_result_out(std::string name_used, bool is_fatal);

        //! \brief Report that something called doesn't exist
        //! \param name_called The name of the calld method
        //! \param line_no Line number
        void report_callee_doesnt_exist(std::string name_called, int line_no);

        //! \brief Report parameter length mismatch
        //! \param caller The name of the caller
        //! \param callee The name of the callee
        //! \param caller_params The number of parameters the caller is attempting to send
        //! \param callee_params The number of parameters the callee is expecting
        //! \param line_no Line number
        void report_mismatched_param_length(std::string caller, std::string callee, uint64_t caller_params, uint64_t callee_params, int line_no);

        //! \brief Report a custom error
        //! \param from Where the error originates
        //! \param error The error to output
        //! \param is_fatal Triggers exit if true
        //! \note This is mainly used for a developer error, really only things that should't happen
        //!       should utilize this
        void report_custom(std::string from, std::string error, bool is_fatal);

        //! \brief Report that a function doesn't have a matching return
        //! \param function The function name / details to output
        //! \param line_no Line number
        //! \post This method is a default fatal that will trigger exit
        void report_no_return(std::string function, int line_no);

        //! \brief Report that a 'main' function was not found
        //! \post This method is a default fatal that will trigger exit
        void report_no_main_function();

        //! \brief Report that a call's return type isn't handled
        //! \param caller_function The function where the call originated
        //! \param callee The function being called
        //! \param is_fatal Triggers exist if true
        void report_calls_return_value_unhandled(std::string caller_function, std::string callee, int line_no, bool is_fatal);

        //! \brief Report syntax error
        //! \param line Line number
        //! \param column Column number
        //! \param error_message Message
        //! \param line_in_question The line that caused the error
        void report_syntax_error(int line, int column, std::string error_message, std::string line_in_question);

        //! \brief Start greater than end error
        //! \param line The line number
        //! \param start The start point
        //! \param end The end point
        //! \note  This is a fatal error
        void report_range_invalid_start_gt_end(int line, std::string start, std::string end);

        //! \brief The given range does nothing
        //! \param line The line number
        //! \param start The start point
        //! \param end The end point
        //! \note  This is a fatal error
        void report_range_ineffective(int line, std::string start, std::string end);

        //! \brief Step command doesn't do anything
        //! \param line The line that the step is on
        void report_invalid_step(int line);

        //! \brief Report that preprocessor couldn't read in a file
        //! \param include_crumbs The file path list that lead to the file
        //! \param file_in_question The file that couldn't be read
        void report_preproc_file_read_fail(std::vector<std::string> include_crumbs, std::string file_in_question);

        //! \brief Report include path not a directory
        //! \param path The path that isn't a directory on the system
        void report_preproc_include_path_not_dir(std::string path);

        //! \brief Report file not found
        //! \param info Info about why we were looking for it
        //! \param file The file name
        void report_preproc_file_not_found(std::string info, std::string file, std::string from);

    private:
        DEL_Driver & driver;

        void display_error_start(bool is_fatal, int line_no=0);
        void display_line_and_error_pointer(std::string line, int column, bool is_fatal, bool show_arrow=true);
    };
}
#endif