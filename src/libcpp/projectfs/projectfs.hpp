/*
    Given a directory, the Project FS class will determine if the target is an application or 
    library for NHLL. It will then load the project file structure in a manner that allows us to
    easily obtain files and configurations relevant to compiling / interpreting 
*/

#ifndef NABLA_PROJECT_FS_HPP
#define NABLA_PROJECT_FS_HPP

#include "json.hpp"
#include "projectstructure.hpp"
#include <string>
#include <vector>

namespace NABLA
{
    //! Project File Structure loader
    class ProjectFS
    {
    public:

        static constexpr char MODULE_ENTRY_FILE[]      = "mod.n";   //! Name for the entry file of a module
        static constexpr char APPLICATION_ENTRY_FILE[] = "main.n";  //! Name for the entry file of an application
        static constexpr char LIBRARY_ENTRY_FILE[]     = "lib.n";   //! Name for the entry file of a library
        static constexpr char UNKNOWN_ENTRY_FILE[]     = "_NABLA_UNKNOWN_FILE_.n"; //! Unknown file
        static constexpr char CONFIG_FILE[]            = "config.json"; //! Project configuration file

        //! \brief Enumeration for types of potential load results
        enum class LoadResultCodes
        {
            OKAY,
            ERROR_GIVEN_PATH_NOT_DIRECTORY,
            ERROR_FAILED_TO_OPEN_CONFIG,
            ERROR_FAILED_TO_LOAD_CONFIG
        };

        //! \brief Construct a ProjectFS
        ProjectFS();

        //! \brief Destruct a ProjectFS
        ~ProjectFS();

        //! \brief Attempt to load a project from a directory
        //! \param location The location of the project
        //! \returns Enumeration detailing load result
        LoadResultCodes load(std::string location);

        //! \brief Check if a project has been loaded
        //! \retval True iff a project is loaded
        bool isLoaded() const;

        //! \brief Get a ProjectStrucure object detailing the information loaded
        //! \returns A project structure with the project information
        //! \pre Ensure that a project is loaded before calling
        ProjectStructure get_project_structure() const;
        
    private:
        void unsetProject();
        bool populateRelativeStructure();

        bool projectLoaded;

        std::string project_location;
        ProjectStructure original_structure;
        ProjectStructure relative_structure;
    };

}

#endif