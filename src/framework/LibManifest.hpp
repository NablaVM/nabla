#ifndef NABLA_LIB_MANIFEST
#define NABLA_LIB_MANIFEST

#include <string>
#include <map>
#include <libnabla/projectfs.hpp>
#include <libnabla/json.hpp>

/*
    Installed libraries are referred to as 'bundles' for each group of libraries.

    For instance :

        Bundle     |   Libraries
        -----------|---------------- 
         stdlib    |    io, net, etc
         bolsey    |    idk

    A manifest.json file is expected in the root folder that contains the bundles that describe-ish the file layout

    < File layout >
    libs
      |--- bundle
      |     | ---- lib
      |     | ----- lib1
      |
      | ------ bundle1
                 | - lib

    < manifest.json >
    {
        "bundle": [
        "lib", "lib1"
        ],
        "bundle1": [
        "lib"
        ]
    }
*/

namespace NABLA
{
    //! \brief Pre-loads installed library information to make parsing / compiling easier. 
    //!        Each library has a ProjectFS representation created that can be used to access
    //!        The installed libs
    class LibManifest
    {
    public:
        //! \brief Construct a Lib Manifest
        LibManifest();

        //! \brief Deconstruct a Lib Manifest
        ~LibManifest();

        //! \brief Load a manifest given a location (Expects a manifest.json)
        //! \param location The disc location that contains a manifest.json file and the libraries to load
        //! \returns true if the libs are loaded correctly, false otherwise
        bool load_manifest(std::string location);

        //! \brief Get a library. 
        //! \param bundle The bundle that the library belongs to
        //! \param lib The library
        //! \param found [out] Will be true if the returned ProjectFS is one from the libraries map
        //! \retval A ProjectFS object that details the library. Will be valid iff found is true
        NABLA::ProjectFS get_library(std::string bundle, std::string lib, bool & found);

    private:

        // Maps Library bundle to individual library objects i.e   stdlib -> io  -> io_project_fs
        //                                                               \-> net -> net_project_fs
        std::map< std::string, std::map<std::string, NABLA::ProjectFS> > libraries;
    };
}

#endif