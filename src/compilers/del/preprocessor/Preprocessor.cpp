#include "Preprocessor.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <regex>

namespace DEL
{
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Preprocessor::Preprocessor(Errors & error_man) : error_man(error_man),
                                                     preproc_ready(false),
                                                     default_path("del.preproc")
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Preprocessor::~Preprocessor()
    {
      //  std::filesystem::remove_all(std::filesystem::path(default_path));
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Preprocessor::process(const char * const filename)
    {
        // Add the directory of the file to the include path
        add_include_path(std::filesystem::path(filename).parent_path().string());

        // Begin processing
        process_file(filename);

        // Check if output file exists, and if it does, remove it
        if(std::filesystem::is_regular_file(std::filesystem::path(default_path)))
        {
            std::filesystem::remove_all(std::filesystem::path(default_path));
        }

        // Open the output file
        std::ofstream out_file( default_path );
        if( ! out_file.good() )
        {
            error_man.report_custom("Preprocessor", " Unable to open output file for preprocessing", true);
        }

        // Write the preprocessed file
        for(auto & pair : pre_processed_pair)
        {
            out_file << pair.line << std::endl;;
        }
        out_file.close();

        preproc_ready = true;
        preprocessed_file = default_path;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Preprocessor::add_include_path(std::string path)
    {
        if(std::find(std::begin(include_paths), std::end(include_paths), path) == std::end(include_paths))
        {
            if(!std::filesystem::is_directory(std::filesystem::path(path)))
            {
                error_man.report_preproc_include_path_not_dir(path);
            }
            include_paths.push_back(path);
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Preprocessor::process_file(std::string file)
    {
        // Indicate that we are currently looking at this file
        current_file_stack.push(file);

        pre_processed_pair.push_back({"@file:" + file, 0});

        // Start reading file
        std::ifstream in_file( file );
        if( ! in_file.good() )
        {
            error_man.report_custom("Preprocessor", " Unable to open input file for preprocessing", true);
        }

        uint64_t line_no = 1;

        // Read each line
        std::string line;
        while(std::getline(in_file, line))
        {
            if(!std::all_of(line.begin(),line.end(),isspace))
            {
                process_line(line, line_no);
            }
            // Count line numbers even if they're blank
            line_no++;
        }

        in_file.close();

        // Add the file as its already processed
        included_files.push_back(current_file_stack.top());

        // Remove this line because we're done
        current_file_stack.pop();

        if(current_file_stack.size() > 0)
        {
            pre_processed_pair.push_back({"@file:" + current_file_stack.top(), 0});
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Preprocessor::process_line(std::string line, uint64_t number)
    {
        /*
            Check for a use statement
        */
        if(std::regex_match(line, std::regex("use\\(\"[a-zA-Z0-9_]+\\.del\"\\)")))
        {
            // Remove spaces
            line.erase(remove_if(line.begin(), line.end(), isspace), line.end());

            // Get the string that indicates a file name (hopefully)
            std::string use_file = line.substr(5, line.size()-7);

            // Go through the include directories given to the preproc and see 
            // if the file can be located
            for(auto & include_dir : include_paths)
            {
                // Make a path from the include dir string
                auto p = std::filesystem::path(include_dir);

                // Add the file given to us to the path
                p /= use_file;

                // Check if the file has already been included
                if(std::find(std::begin(included_files), std::end(included_files), p.string()) != std::end(included_files))
                {
                    return;
                }

                // If its a file, we use it
                if(std::filesystem::is_regular_file(p))
                {
                    // Start the processing
                    process_file(p.string());
                    return;
                }
            }

            // If we didn't return, then.. well.. thats a problem
            error_man.report_preproc_file_not_found("Unable to locate file for use statement", use_file, current_file_stack.top());
        }

        /*
            Add the line to the final list
        */
        pre_processed_pair.push_back({line, number});
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string Preprocessor::get_preprocessed_filename() const
    {
        if(!preproc_ready)
        {
            // This is a developer error
            error_man.report_custom("Preprocessor", " get_preprocessed_filename called before preprocessor was ran", true);
        }

        return preprocessed_file;
    }

    std::string Preprocessor::fetch_line(int line_number)
    {
        if(!preproc_ready)
        {
            // This is a developer error
            error_man.report_custom("Preprocessor", " fetch_line called before preprocessor was ran", true);
        }

        if(line_number > 0)  { line_number = line_number-1; }
        if(line_number > pre_processed_pair.size() || line_number < 0)
        {
            // This is a developer error
            error_man.report_custom("Preprocessor", " fetch_line requested line out of range", true);
        }

        return pre_processed_pair[line_number].line;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    uint64_t Preprocessor::fetch_user_line_number(int line_number)
    {
        if(!preproc_ready)
        {
            // This is a developer error
            error_man.report_custom("Preprocessor", " fetch_user_line_number called before preprocessor was ran", true);
        }

        if(line_number > 0)  { line_number = line_number-1; }
        if(line_number > pre_processed_pair.size() || line_number < 0)
        {
            // This is a developer error
            error_man.report_custom("Preprocessor", " fetch_user_line_number requested line out of range", true);
        }

        return pre_processed_pair[line_number].number;
    }
}