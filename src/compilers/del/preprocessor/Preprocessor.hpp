#ifndef DEL_PRE_PROCESSOR_HPP
#define DEL_PRE_PROCESSOR_HPP

#include "Errors.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <stack>

namespace DEL
{
    class Preprocessor
    {
    public:
        Preprocessor(Errors & error_man);
        ~Preprocessor();

        void process(const char * const filename);

        std::string get_preprocessed_filename() const;

        void add_include_path(std::string path);

        std::string fetch_line(int line_number);
        uint64_t    fetch_user_line_number(int line_number);

    private:

        void process_file(std::string file);
        void process_line(std::string line, uint64_t number);

        struct line_no_pair
        {
            std::string line;
            uint64_t    number;
        };

        std::vector<line_no_pair> pre_processed_pair;   // Resulting preprocessed file with original line indexing
        std::vector<std::string> include_paths;         // Paths to search for use statements

        std::stack<std::string> current_file_stack;     // File processing stack for generating @file directives
        std::vector<std::string> included_files;        // Files already included

        Errors & error_man;
        bool preproc_ready;
        std::string default_path;
        std::string preprocessed_file;
    };
}

#endif 