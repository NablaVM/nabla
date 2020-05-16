#ifndef NABLA_PROJECT_FS_PROJECT_STRUCTURE_HPP
#define NABLA_PROJECT_FS_PROJECT_STRUCTURE_HPP

#include "projecttype.hpp"
#include "json.hpp"
#include <string>
#include <vector>

namespace NABLA
{
    //! \brief FWD For ProjectFS Class
    class ProjectFS;
    
    //! \brief A Class to detail a project's structure
    class ProjectStructure
    {
    public:

        //! \brief Create PS
        ProjectStructure();

        //! \brief Destroy a PS
        ~ProjectStructure();

        //! \brief Get the author data for the project
        //! \returns String containing information
        std::string get_author() const;

        //! \brief Get the description data for the project
        //! \returns String containing information
        std::string get_description() const;
        
        //! \brief Get the project type data for the project
        //! \returns Enum containing project type
        ProjectType get_projectType() const;
        
        //! \brief Get the entry file for the project
        //! \returns String containing information
        std::string get_entryFile() const;
        
        //! \brief Get the list of module entry files
        //! \returns Vector of paths for module entry files
        std::vector<std::string> get_module_list() const;

    private:
        friend ProjectFS;
        friend inline void to_json(njson::json& json_config, const ProjectStructure& project);
        friend inline void from_json(const njson::json& json_config, ProjectStructure& project);

        std::string author;
        std::string description;
        ProjectType projectType;
        std::string entryFile;
        std::vector<std::string> module_list;

        void clear()
        {  
            this->projectType = ProjectType::UNKNOWN;
            this->author = "";
            this->description = "";
            this->entryFile = "";
            this->module_list.clear();
        }
    };

    // Convert project structure to json
    inline void to_json(njson::json& json_config, const ProjectStructure& project)
    {
        json_config["author"]       = project.author;
        json_config["description"]  = project.description;
        json_config["modules"]      = project.module_list;

        switch(project.projectType)
        {
            case ProjectType::LIBRARY:
                json_config["project_type"] = "lib";
                break;
            case ProjectType::APPLICATION:
                json_config["project_type"] = "app";
                break;
            case ProjectType::UNKNOWN:
                json_config["project_type"] = "unknown";
            default:
                json_config["project_type"] = "unknown";
                break;
        };
    }

    // Convert json object to project structure
    inline void from_json(const njson::json& json_config, ProjectStructure& project) 
    {
        json_config.at("author").get_to(project.author);
        json_config.at("description").get_to(project.description);
        json_config.at("modules").get_to(project.module_list);

        std::string project_type;
        json_config.at("project_type").get_to(project_type);

        if(project_type == "app") 
        { 
            project.projectType = ProjectType::APPLICATION;
        }
        else if(project_type == "lib")
        { 
            project.projectType = ProjectType::LIBRARY;
        }
        else 
        {
            project.projectType = ProjectType::UNKNOWN;
        }
    }
}

#endif