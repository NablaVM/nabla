# Project FS

The Project FS' goal is to load a library or application directory and ensure the following : 

1. The given path is a directory containing a config.json that is valid json detailing the project
2. The project type-specific entry file (lib.n or main.n) exists in the root of the project directory
3. All listed modules from config.json have a corresponding directory
4. Each of those directories contain a mod.n file

Once all of these are checked a ProjectStructure object is populated with the relative paths to all files associated with
the project. 

**Example:** 

    example_application
        |
        |--------- module_one
        |               | ------ someFile.n
        |               | ------ mod.n
        |
        |--------- module_two
        |               | ------ someFile.n
        |               | ------ mod.n
        |
        |--- config.json
        |--- main.n

Will build a ProjectStruture giving the information :

1. project type (app, lib)
2. author
3. description
4. path to main.n
5. path to module_one/mod.n
6. path to module_two/mod.n

This will enable the caller to parse the **.n** files and use the relative paths to locate module-specific nabla source.