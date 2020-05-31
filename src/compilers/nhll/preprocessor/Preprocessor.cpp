/*
    NEEDS_UPDATE


    This is not the most elegant implementation. I started writing this and then got annoyed that I had to do this so its kind of mush. 
    Redoing this is definitely warranted. As long as the same output is produced, no other parts of the system will have to be redone
*/

#include "Preprocessor.hpp"
#include <iostream>
#include <filesystem>
#include <sstream>
#include <regex>
#include <libnabla/json.hpp>

namespace NHLL
{
    namespace
    {
        //  Setup the build directory
        //
        bool setup_build_dir()
        {
            std::filesystem::path path{ ".nhll_build" };
            path /= "preproc.n";

            if(!std::filesystem::create_directories(path.parent_path()))
            {
                if(!std::filesystem::remove_all(path.parent_path()))
                {
                    return false;
                }

                if(!std::filesystem::create_directories(path.parent_path()))
                {
                    return false;
                }
            }
            return true;
        }

        //  Slice up something by a char
        //
        std::vector<std::string> split_by(std::string line, char c)
        {
            std::string part;
            std::istringstream ss(line);
            std::vector<std::string> parts;
            while(std::getline(ss, part, c))
            {
                parts.push_back(part);
            }
            return parts;
        }

        //  Remove Spaces
        //
        void remove_spaces(std::string &str)
        {
            auto f = [](unsigned char const c) { return std::isspace(c); };
            str.erase(std::remove_if(str.begin(), str.end(), f), str.end());
        }
    }
    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    Preprocessor::Preprocessor(NABLA::LibManifest & lm, bool verbose) : 
                                                          verbose(verbose),
                                                          completed(false), 
                                                          manifest(lm)
    {
        token_regex = {
            MatchCall{ std::regex("(^|\\s*)#import.*"), PreProcToken::IMPORT  },
            MatchCall{ std::regex("(^|\\s*)#export.*"), PreProcToken::EXPORT  }
        };
    }

    Preprocessor::~Preprocessor()
    {
        token_regex.clear();
    }

    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    bool Preprocessor::process(NABLA::ProjectFS & project)
    {
        //  Setup the build directory
        //
        if(!setup_build_dir())
        {
            std::cerr << "Preprocessor::Error : " << "Unable setup build dir " << std::endl;
            return false;
        }


        output_path = ".nhll_build/preproc.n";

        output.open(output_path);

        if(!output.is_open())
        {
            std::cerr << "Preprocessor::Error : Unable to open preproc output destination" << std::endl;
            return false;
        }

        NABLA::ProjectStructure ps = project.get_project_structure();

        if(completed)
        {
            std::cerr << "Preprocessor::Error : Must be reset before pre-processing again" << std::endl;
            return false;
        }

        completed = process_project_structure(ps) ;

        output.close();
        return completed;
    }

    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    std::string Preprocessor::get_preproc() const
    {
        return output_path;
    }

    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    void Preprocessor::reset()
    {
        completed  = false;
        output.close();
    }

    // ------------------------------------------------
    //
    //  Take in a project strucure and look for imports. If an import is found we call handle_import
    //  Otherwise, we write the line out to the preproc file
    //
    // ------------------------------------------------
    
    bool Preprocessor::process_project_structure(NABLA::ProjectStructure ps)
    {
        if(verbose) { std::cout << "Preprocessor::Info : " << "Starting process of project \"" << ps.get_entryFile() << "\"" << std::endl; }

        //  Open the entry file
        //
        std::ifstream ifs(ps.get_entryFile());

        if(!ifs.is_open())
        {
            std::cerr << "Preprocessor::Error : " << "Unable to open : " << ps.get_entryFile() << std::endl;
            return false;
        }

        std::string line;
        while(std::getline(ifs, line))
        {
            bool matched = false;
            for(auto & rm : token_regex)
            {
                if(std::regex_match(line, rm.reg))
                {
                    switch(rm.token)
                    {
                    case PreProcToken::IMPORT: 
                    {
                        matched = true;
                        remove_spaces(line);

                        // Remove token from line, and attempt to 'import'
                        if(!handle_import(ps, line.substr(line.find("#import") + 7))) 
                        { 
                            return false; 
                        }
                        break;
                    }
                    case PreProcToken::EXPORT:
                    {
                        std::cerr << "Preprocessor::Error : Unexpected 'EXPORT' directive. These should only be in mod.n files" << std::endl;
                        std::cerr << "----> Did you mean to use \"#import\"?" << std::endl;
                        return false;
                    }
                    default : return false;
                    }
                }
            }

            if(!matched)
            {
                output << line + "\n";
            }
        }

        return true;
    }

    // ------------------------------------------------
    //
    //  Find the module path within the project structure that module_entry is referring to
    //  If found pass the file to process_file where imports can happen
    //
    //  This operates on (mod.n) files and exports whatever they wanted exported
    //
    // ------------------------------------------------

    bool Preprocessor::process_module_file(NABLA::ProjectStructure & ps, std::string module_entry)
    {
        // Pull the module list and attempt to locate the file that contains the information for the import 
        //
        std::vector<std::string> module_list = ps.get_module_list();

        bool found = false;
        std::string module_path;
        for(auto & mod : module_list)
        {
            auto path = std::filesystem::path(mod);
            std::string module_name = path.parent_path().filename();
            if(module_name ==  module_entry)
            {
                module_path = mod;
                found = true;
            }
        }

        //  Module not found
        //
        if(!found)
        {
            std::cerr << "Preprocessor::Error : Unable to locate mod.n file for module \"" << module_entry << "\"" << std::endl;
            return false;
        }

        //  Open the entry file
        //
        std::ifstream ifs(module_path);

        if(!ifs.is_open())
        {
            std::cerr << "Preprocessor::Error : " << "Unable to open : " << module_path << std::endl;
            return false;
        }

        std::string line;
        while(std::getline(ifs, line))
        {
            for(auto & rm : token_regex)
            {
                if(std::regex_match(line, rm.reg))
                {
                    switch(rm.token)
                    {
                    case PreProcToken::IMPORT: 
                    {
                        std::cerr << "Preprocessor::Error : Unexpected 'IMPORT' directive in module file. 'IMPORT' should not be present in mod.n files" << std::endl;
                        return false;
                    }
                    case PreProcToken::EXPORT:
                    {
                        remove_spaces(line);

                        //  Build expected file name
                        //
                        auto file_path = std::filesystem::path(module_path).parent_path();

                        file_path /= (line.substr(line.find("#export") + 7) + ".n");

                        if(!std::filesystem::is_regular_file(file_path))
                        {
                            std::cerr << "Preprocessor::Error : Can not locate module item for export. Expected \"" << file_path.string() << "\" for export : " << line << std::endl; 
                            return false;
                        }

                        // Remove token from line, and attempt to 'export'
                        if(!process_file(ps, file_path.string()))
                        { 
                            return false; 
                        }
                        break;
                    }
                    default : return false;
                    }
                }
            }
        }


        return true;
    }

    // ------------------------------------------------
    //
    //  Process a file that was exported from a mod.n file
    //
    // ------------------------------------------------
    
    bool Preprocessor::process_file(NABLA::ProjectStructure & ps, std::string file)
    {
        //  Open the entry file
        //
        std::ifstream ifs(file);

        if(!ifs.is_open())
        {
            std::cerr << "Preprocessor::Error : " << "Unable to open : " << file << std::endl;
            return false;
        }

        std::string prefix_for_functions;
        auto path = std::filesystem::path(file);

        if(ps.get_projectType() == NABLA::ProjectType::APPLICATION)
        {
            prefix_for_functions = path.parent_path().filename().string() + ".";
        }
        else if (ps.get_projectType() == NABLA::ProjectType::LIBRARY)
        {
            std::string tmp = path.parent_path().filename();
            prefix_for_functions = path.parent_path().parent_path().filename().string() + "." + tmp + ".";
        }
        else
        {
            std::cerr << "Preprocessor::Error : Unable to determine project type for \" " << file << "\"" << std::endl;
            return false;
        }

        std::string line;
        while(std::getline(ifs, line))
        {
            bool matched = false;
            for(auto & rm : token_regex)
            {
                if(std::regex_match(line, rm.reg))
                {
                    switch(rm.token)
                    {
                    case PreProcToken::IMPORT: 
                    {
                        matched = true;
                        remove_spaces(line);

                        // Remove token from line, and attempt to 'import'
                        if(!handle_import(ps, line.substr(line.find("#import") + 7))) 
                        { 
                            return false; 
                        }
                        break;
                    }
                    case PreProcToken::EXPORT:
                    {
                        std::cerr << "Preprocessor::Error : Unexpected 'EXPORT' directive. These should only be in mod.n files" << std::endl;
                        std::cerr << "----> Did you mean to use \"#import\"?" << std::endl;
                        return false;
                    }
                    default : return false;
                    }
                }
            }


            //  If the line is a 'def someFunc' we need to append the prefix
            //
            if(std::regex_match(line, std::regex("(^|\\s*)def(\\s*).*")))
            {
                line  = std::regex_replace(line, std::regex("(^|\\s*)def(\\s*)"), ("def " + prefix_for_functions), std::regex_constants::format_first_only);
            }

            //  If the line is a 'global someVar' we need to append the prefix
            //
            if(std::regex_match(line, std::regex("(^|\\s*)global(\\s*).*")))
            {
                line  = std::regex_replace(line, std::regex("(^|\\s*)global(\\s*)"), ("global " + prefix_for_functions), std::regex_constants::format_first_only);
            }

            //  If the line wasnt an import we write out
            //
            if(!matched)
            {
                output << line + "\n";
            }
        }

        return true;
    }

    // ------------------------------------------------
    //
    //  Handle an import by finding where the module is suppose to be from. First we check 'local' meaning the user defined it in their project
    //  If the 'local' was not prefixed on the import, then we ask the library manifest for the library as we assume it is stored on the system
    //
    // ------------------------------------------------
    
    bool Preprocessor::handle_import(NABLA::ProjectStructure & ps, std::string line)
    {
        //  Check if import has already occured
        //
        for(auto & imported : handled_imports)
        {
            if(imported == line)
            {
                if(verbose) { std::cout << "Preprocessor::Info : \"" << line << "\"" << " already imported" << std::endl; }
                return true;
            }
        }

        //  Assume that the import will be just fine and add it to the imported objects list
        //
        handled_imports.push_back(line);

        //  Split the import command up to determine what we need to do with it
        //
        std::vector<std::string> import_parts = split_by(line, '.');

        if(import_parts.size() < 2)
        {
            std::cerr << "Preprocessor::Error : Import does not identify a target" << std::endl;
            return false;
        }

        //  Check for local import first
        //
        if(import_parts[0] == "local")
        {
            std::vector<std::string> module_list = ps.get_module_list();

            for(auto & mod : module_list)
            {
                auto path = std::filesystem::path(mod);
                std::string module_name = path.parent_path().filename();

                if(module_name ==  import_parts[1])
                {
                    return process_module_file(ps, path.parent_path().filename());
                }
            }

            std::cerr << "Preprocessor::Error : Local import failed to find \"" << import_parts[1] << "\"" << std::endl;
            return false;
        }

        //  Check the manifest for the requested library
        //
        bool found = false;
        NABLA::ProjectFS lib_ps = manifest.get_library(import_parts[0], import_parts[1], found);

        if(!found)
        {
            std::cerr << "Preprocessor::Error : Unable to find module \"" << import_parts[0] << "." << import_parts[1] << "\"." << std::endl;
            return false;
        }

        return process_project_structure(lib_ps.get_project_structure());
    }

}