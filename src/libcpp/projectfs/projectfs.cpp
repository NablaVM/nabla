#include "projectfs.hpp"
#include "projecttype.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace NABLA
{
    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    ProjectFS::ProjectFS() : projectLoaded(false)
    {
        original_structure.clear();
        relative_structure.clear();
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    ProjectFS::~ProjectFS()
    {
        
    }
    
    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    void ProjectFS::unsetProject()
    {
        projectLoaded = false;
        original_structure.clear();
        relative_structure.clear();
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    bool ProjectFS::isLoaded() const
    {
        return projectLoaded;
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    ProjectStructure ProjectFS::get_project_structure() const
    {
        return relative_structure;
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    bool ProjectFS::populateRelativeStructure()
    {
        if(!projectLoaded){ return false; }

        relative_structure.author       = original_structure.author;
        relative_structure.description  = original_structure.description;
        relative_structure.projectType  = original_structure.projectType;

        //  Ensure project entry file exists
        //
        std::string expected_file;
        auto entry_path = std::filesystem::path(project_location); 
        switch(original_structure.projectType)
        {
            case ProjectType::LIBRARY:     entry_path /= LIBRARY_ENTRY_FILE;     expected_file = LIBRARY_ENTRY_FILE;      break;
            case ProjectType::APPLICATION: entry_path /= APPLICATION_ENTRY_FILE; expected_file = APPLICATION_ENTRY_FILE; break;
            case ProjectType::UNKNOWN:     
                std::cerr << "Project type not able to be determined from " << CONFIG_FILE << std::endl;
                return false;
            default:
                std::cerr << "Project type not able to be determined from " << CONFIG_FILE << std::endl;
                return false;
        };

        if(!std::filesystem::is_regular_file(std::filesystem::status(entry_path.string())))
        {
            std::cerr << "Error: Expected entry file \"" << expected_file << "\" in \"" << project_location << "\"." << std::endl;
            return false; 
        }

        //  Set the file path
        //
        relative_structure.entryFile = entry_path.string();

        //  Check all given modules and ensure they exist with entry files
        //
        for(auto &module : original_structure.module_list)
        {
            auto path = std::filesystem::path(project_location);
            path /= module;

            // Make sure that the specified path exists as a directory
            if(!std::filesystem::is_directory(std::filesystem::status(path.string())))
            {
                std::cerr << "Error : Module \"" 
                          << module
                          << "\" doesn't appear to exist as a directory"
                          << std::endl;
                return false;
            }

            // Add the entry file and check that it also exists
            path /= MODULE_ENTRY_FILE;

            if(!std::filesystem::is_regular_file(std::filesystem::status(path.string())))
            {
                std::cerr << "Error : Module \"" 
                          << module
                          << "\" doesn't appear to have a module entry file. Expected: " 
                          << MODULE_ENTRY_FILE 
                          << std::endl;
                return false;
            }

            //  Set the module path
            //
            relative_structure.module_list.push_back(path.string());
        }

        return true;
    }
    
    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    ProjectFS::LoadResultCodes ProjectFS::load(std::string location)
    {
        unsetProject();

        // Ensure what they gave us is a directory
        if(!std::filesystem::is_directory(std::filesystem::status(location)))
        {
            return LoadResultCodes::ERROR_GIVEN_PATH_NOT_DIRECTORY;
        }

        // Add config.json to the path .. using /= ?? Okay..
        auto path = std::filesystem::path(location);
        path /= CONFIG_FILE;

        // Open the json file
        std::ifstream ifs(path);
        if(!ifs.is_open()) 
        {
            return LoadResultCodes::ERROR_FAILED_TO_OPEN_CONFIG;
        }

        // Attempt to load the json - We can add particular catches later
        njson::json configuration_file;
        try
        {
            ifs >> configuration_file;

            from_json(configuration_file, original_structure);
        }
        catch(...)
        {
            return LoadResultCodes::ERROR_FAILED_TO_LOAD_CONFIG;
        }

        projectLoaded = true;
        project_location = location;

        if(!populateRelativeStructure())
        {
            projectLoaded = false;
            return LoadResultCodes::ERROR_FAILED_TO_LOAD_CONFIG;
        }

        return LoadResultCodes::OKAY;
    }
}