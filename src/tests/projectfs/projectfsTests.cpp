
#include "projecttype.hpp"
#include "projectstructure.hpp"
#include "projectfs.hpp"

#include <iostream>
#include <random>
#include <filesystem>
#include <fstream>
#include "CppUTest/TestHarness.h"

namespace
{
    bool setup_directory_structure(std::string location)
    {
        auto path = std::filesystem::path(location);

        std::filesystem::create_directory(path);
        if(!std::filesystem::is_directory(std::filesystem::status(path.string()))){ return false; }

        path /= "module_one";
        std::filesystem::create_directory(path);
        if(!std::filesystem::is_directory(std::filesystem::status(path.string()))){ return false; }

        path = path.parent_path();
        path /= "module_two";
        std::filesystem::create_directory(path);
        if(!std::filesystem::is_directory(std::filesystem::status(path.string()))){ return false; }

        return true;
    }

    bool write_to_test_file(std::string file, std::string data)
    {
        std::ofstream  f(file);

        if(!f.is_open()) { return false; }

        f << data;
        f.close();
        
        return true;
    }

    bool setup_application_files(std::string location)
    {
        std::string config_file = 
        "{ \"author\": \"Josh A. Bosley\",  \"description\": \"A test application\", \"project_type\": \"app\",  \"modules\": [  \"module_one\",  \"module_two\"  ] }";

        auto application_path = std::filesystem::path(location);
        auto conf_file_path   = std::filesystem::path(location); conf_file_path  /= "config.json";
        auto main_file_path   = std::filesystem::path(location); main_file_path  /= "main.n";
        auto module_one_path  = std::filesystem::path(location); module_one_path /= "module_one"; module_one_path /= "mod.n";
        auto module_two_path  = std::filesystem::path(location); module_two_path /= "module_two"; module_two_path /= "mod.n";

        if(!write_to_test_file(conf_file_path, config_file)) { return false; }
        if(!write_to_test_file(main_file_path,  "Some text")){ return false; }
        if(!write_to_test_file(module_one_path, "Some text")){ return false; }
        if(!write_to_test_file(module_two_path, "Some text")){ return false; }
        return true;
    }
    
    bool setup_library_files(std::string location)
    {
        std::string config_file = 
        "{ \"author\": \"Josh A. Bosley\",  \"description\": \"A test library\", \"project_type\": \"lib\",  \"modules\": [  \"module_one\",  \"module_two\"  ] }";

        auto application_path = std::filesystem::path(location);
        auto conf_file_path   = std::filesystem::path(location); conf_file_path  /= "config.json";
        auto main_file_path   = std::filesystem::path(location); main_file_path  /= "lib.n";
        auto module_one_path  = std::filesystem::path(location); module_one_path /= "module_one"; module_one_path /= "mod.n";
        auto module_two_path  = std::filesystem::path(location); module_two_path /= "module_two"; module_two_path /= "mod.n";

        if(!write_to_test_file(conf_file_path, config_file)) { return false; }
        if(!write_to_test_file(main_file_path,  "Some text")){ return false; }
        if(!write_to_test_file(module_one_path, "Some text")){ return false; }
        if(!write_to_test_file(module_two_path, "Some text")){ return false; }
        return true;
    }
    
    bool setup_failure_files(std::string location)
    {
        std::string config_file = 
        "{ A case of some bad json \"A test application\", \"project_type\": \"app\",  \"modules\": [  \"module_one\",  \"module_two\"  ] }";

        auto application_path = std::filesystem::path(location);
        auto conf_file_path   = std::filesystem::path(location); conf_file_path  /= "config.json";
        auto main_file_path   = std::filesystem::path(location); main_file_path  /= "main.n";
        auto module_one_path  = std::filesystem::path(location); module_one_path /= "module_one"; module_one_path /= "mod.n";
        auto module_two_path  = std::filesystem::path(location); module_two_path /= "module_two"; module_two_path /= "mod.n";

        if(!write_to_test_file(conf_file_path, config_file)) { return false; }
        if(!write_to_test_file(main_file_path,  "Some text")){ return false; }
        if(!write_to_test_file(module_one_path, "Some text")){ return false; }
        if(!write_to_test_file(module_two_path, "Some text")){ return false; }
        return true;
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(ProjectFsTests)
{   
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(ProjectFsTests, failuresToLoad)
{
    std::string location = "test_failures";

    CHECK_TRUE(setup_directory_structure(location));
    CHECK_TRUE(setup_failure_files(location));

    NABLA::ProjectFS pfs; 

    auto conf_file_path   = std::filesystem::path(location); conf_file_path  /= "config.json";
    
    CHECK_TRUE(NABLA::ProjectFS::LoadResultCodes::ERROR_GIVEN_PATH_NOT_DIRECTORY == pfs.load(conf_file_path.string()));

    CHECK_TRUE(NABLA::ProjectFS::LoadResultCodes::ERROR_FAILED_TO_LOAD_CONFIG    == pfs.load(location));
    
    CHECK_TRUE(std::filesystem::remove(conf_file_path));

    CHECK_TRUE(NABLA::ProjectFS::LoadResultCodes::ERROR_FAILED_TO_OPEN_CONFIG    == pfs.load(location));
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(ProjectFsTests, applicationExpectedPassTest)
{
    std::string location = "test_application";

    CHECK_TRUE(setup_directory_structure(location));
    CHECK_TRUE(setup_application_files(location));

    NABLA::ProjectFS pfs; 
    
    CHECK_TRUE(NABLA::ProjectFS::LoadResultCodes::OKAY == pfs.load(location));

    CHECK_TRUE(pfs.isLoaded());

    NABLA::ProjectStructure ps = pfs.get_project_structure();

    CHECK_TRUE(ps.get_author()      == "Josh A. Bosley");
    CHECK_TRUE(ps.get_description() == "A test application");
    CHECK_TRUE(ps.get_projectType() == NABLA::ProjectType::APPLICATION);
    
    auto main_file_path   = std::filesystem::path(location); main_file_path  /= "main.n";
    auto module_one_path  = std::filesystem::path(location); module_one_path /= "module_one"; module_one_path /= "mod.n";
    auto module_two_path  = std::filesystem::path(location); module_two_path /= "module_two"; module_two_path /= "mod.n";

    std::vector<std::string> expected_entry; expected_entry.push_back(module_one_path.string()); expected_entry.push_back(module_two_path.string());

    std::vector<std::string> actual_entry = ps.get_module_list();

    CHECK_TRUE(ps.get_entryFile() == main_file_path.string());

    CHECK_TRUE(expected_entry.size() == actual_entry.size());

    CHECK_TRUE(std::equal(expected_entry.begin(), expected_entry.end(), actual_entry.begin()));
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(ProjectFsTests, libraryExpectedPassTest)
{
    std::string location = "test_library";

    CHECK_TRUE(setup_directory_structure(location));
    CHECK_TRUE(setup_library_files(location));

    NABLA::ProjectFS pfs; 
    
    CHECK_TRUE(NABLA::ProjectFS::LoadResultCodes::OKAY == pfs.load(location));

    CHECK_TRUE(pfs.isLoaded());

    NABLA::ProjectStructure ps = pfs.get_project_structure();

    CHECK_TRUE(ps.get_author()      == "Josh A. Bosley");
    CHECK_TRUE(ps.get_description() == "A test library");
    CHECK_TRUE(ps.get_projectType() == NABLA::ProjectType::LIBRARY);
    
    auto main_file_path   = std::filesystem::path(location); main_file_path  /= "lib.n";
    auto module_one_path  = std::filesystem::path(location); module_one_path /= "module_one"; module_one_path /= "mod.n";
    auto module_two_path  = std::filesystem::path(location); module_two_path /= "module_two"; module_two_path /= "mod.n";

    std::vector<std::string> expected_entry; expected_entry.push_back(module_one_path.string()); expected_entry.push_back(module_two_path.string());

    std::vector<std::string> actual_entry = ps.get_module_list();

    CHECK_TRUE(ps.get_entryFile() == main_file_path.string());

    CHECK_TRUE(expected_entry.size() == actual_entry.size());

    CHECK_TRUE(std::equal(expected_entry.begin(), expected_entry.end(), actual_entry.begin()));
};