#ifndef NHLL_PREPROCESSOR_HPP
#define NHLL_PREPROCESSOR_HPP

#include <string>
#include <regex>
#include <fstream>
#include "LibManifest.hpp"
#include <libnabla/projectfs.hpp>

namespace NHLL
{
    //! \brief The NHLL Preprocessor
    class Preprocessor
    {
    public:

        //!\brief Construct a preprocessor
        //!\param lm The library manifest for system libs
        //!\param verbose (optional) Enable verbosity
        Preprocessor(NABLA::LibManifest &lm, bool verbose=false);

        //!\brief Deconstruct the preprocessor
        ~Preprocessor();

        //!\brief Process a given project
        //!\retval True process is complete, False otherwise
        bool process(NABLA::ProjectFS & project);

        //!\brief Get the preproc file location
        //!\returns File path to item that can be compiled
        //!\note process() should be called and return true if you expect this to work
        std::string get_preproc() const;

        //! \brief Close things down, and reset the preprocessor
        void reset();

    private:
        enum class PreProcToken
        {
            IMPORT,
            EXPORT
        };

        struct MatchCall
        {
            std::regex reg;
            PreProcToken token;
        };

        bool verbose;
        bool completed;
        NABLA::LibManifest &manifest;
        std::string output_path;

        std::ofstream output;

        std::vector<std::string> global_pool;
        std::vector<std::string> line_pool;

        //  Process a project structure (starting at main.n or lib.n)
        //
        bool process_project_structure(NABLA::ProjectStructure ps);

        //  Process a module file (mod.n)
        //
        bool process_module_file(NABLA::ProjectStructure & ps, std::string module_entry);

        //  Process an individual file within a module
        //
        bool process_file(NABLA::ProjectStructure & ps, std::string file);

        bool handle_import(NABLA::ProjectStructure & ps, std::string line);

        std::vector<MatchCall> token_regex;

        std::vector<std::string> handled_imports;

    };
}


#endif