#include "LibManifest.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>

#include <libnabla/json.hpp>

namespace NABLA
{
    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    LibManifest::LibManifest()
    {

    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------
    
    LibManifest::~LibManifest()
    {

    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------
    
    bool LibManifest::load_manifest(std::string location)
    {
        auto path = std::filesystem::path(location);

        path /= "manifest.json";


        if(!std::filesystem::is_regular_file(std::filesystem::status(path.string())))
        {
            std::cerr << "LibManifest : manifest.json not found in given location : " << location << std::endl;
            return false;
        }

        std::ifstream ifs(path);

        if(!ifs.is_open())
        {
            std::cerr << "LibManifest : Unable to open manifest file : " << path.string() << std::endl;
        }

        njson::json configuration_file;
        try
        {
            ifs >> configuration_file;
        }
        catch(...)
        {
            std::cerr << "Error loading manifest.json. Probable syntax error" << std::endl;
            return false;
        }

        ifs.close();

        // Go through all libary bundles
        //
        for( auto & bundle : configuration_file.items() )
        {
            //  Ensure that the bundle corresponds to a directory
            //
            auto bundle_path = std::filesystem::path(location);
            bundle_path /= bundle.key();

            // Ensure that the listed bundle exists
            if(!std::filesystem::is_directory(std::filesystem::status(bundle_path.string())))
            {
                std::cerr << "Manifest library bundle \"" << bundle.key() << "\"" << "doesn't appear to have a corresponding directory" << std::endl;
                libraries.clear();
                return false;
            }
            
            //  Attempt to load each of the bundle's libraries
            //
            for(auto & lib : bundle.value().items())
            {
                //  Ensure that the library path exists
                //
                auto lib_path = ( bundle_path / lib.value() );

                if(!std::filesystem::is_directory(std::filesystem::status(lib_path.string())))
                {
                    std::cerr << "LibManifest : Library \"" << lib.value() << "\" in bundle \"" << bundle.key() << "\"" << " does not appear to exist " << std::endl;
                    return false;
                }

                //  Attempt to load the library
                //
                NABLA::ProjectFS pfs;

                switch(pfs.load(lib_path.string()))
                {
                    case NABLA::ProjectFS::LoadResultCodes::OKAY:
                        break;

                    case NABLA::ProjectFS::LoadResultCodes::ERROR_GIVEN_PATH_NOT_DIRECTORY:
                        std::cerr << "LibManifest : [" << lib_path.string() << "] Is not a directory" << std::endl;
                        return false;

                    case NABLA::ProjectFS::LoadResultCodes::ERROR_FAILED_TO_OPEN_CONFIG:
                        std::cerr << "LibManifest : Unable to open config.json " << "for \"" << bundle.key() << "::" << lib.value() << "\"" << std::endl;
                        return false;

                    case NABLA::ProjectFS::LoadResultCodes::ERROR_FAILED_TO_LOAD_CONFIG:
                        std::cerr << "LibManifest : Unable to load config.json " << "for \"" << bundle.key() << "::" << lib.value() << "\"" << std::endl;
                        return false;
                }

                //  The thing is finally loaded
                //
                libraries[bundle.key()].insert( std::make_pair(lib.value().get<std::string>(), pfs) );
            }
        }

        return true;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------
    
    NABLA::ProjectFS LibManifest::get_library(std::string bundle, std::string lib, bool & okay)
    {
        if ( libraries.find(bundle) == libraries.end() ) 
        {
            okay = false;
            return NABLA::ProjectFS();
        }

        if(libraries[bundle].find(lib) == libraries[bundle].end())
        {
            okay = false;
            return NABLA::ProjectFS();
        }

        okay = true;
        return libraries[bundle][lib];
    }

}