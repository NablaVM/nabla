# System layout

```

~/.nabla
    |
    |---- manifest.json
    |
    |---- nabla_app
    |
    |---- harp_app
    |
    |---- solace_app
    |
    |---- stdlib
    |       |
    |       |----- io
    |       |----- net
    |       |----- etc
    |
    | --- extlibs
            |
            | ---- coolLib
            | ---- awesomeJson
```

### Location

The location of the nabla working directory doesn't matter, as long as the environment variable 
**NABLA_SYSTEM_DIR** is set. 

**manifest.json**

A json file detailing all of the installed external and std libraries on the system. This will be used by the nabla app to locate installed libraries. 

