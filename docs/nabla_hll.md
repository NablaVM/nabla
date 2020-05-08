# High Level Language

The Nabla High Level Language (NHLL, or HLL) is the top level language that will be compiled to Nabla byte code by the Nabla Compiler. 


## Project layout

```
Project Directory
    |
    |----modules
    |       |----- something.n
    |       |----- somethingelse.n
    |       |----- mod.n
    |
   main.n
   config.json
```

Each project will be in a directory, with the files _main.n_ and _config.json_.

### config.json

The configuration for the project. This file will contain the author, description, version, and dependencies.
Dependencies should indicate where to get them (File system location, git, etc)

### main.n

Main will contain the entry method and any supporting application code. 

### Modules

Each module will contain a _mod.n_ that indicates include order and any forwards that need to take place for the module. Each file in the module will be
parsed in the order that they are listed in _mod.n_.

# Example 

This is a rough example. Syntax is invalid for now as the syntax has not yet been decided on for the NHLL

**mod.n**

```

public something.my_method

public somethingelse.another_method

```

**something.n**

```
def my_method() -> nil {

    // Do something
}

```

**somethingelse.n**

```
def another_method() -> int {

    something.my_method(); // Able to be called because something.n is listed before this in mod.n

    return 4;
}
```

**main.n**

```

// Libs and mods not explicitly stated here but brought in from modules, or config
// are still available. 

use mod.something as st; 

use lib.somelib as lsl; // Brought in from config.json

// std library does not need to be listed in config, and instead get brought in
// if stated as follows:

use std.io; // Now can be used as std.io.println

use std.io as io; // Now can be used as io.println

def main() -> int {

    st.my_method(); // With a use as

    mod.somethingelse.another_method(); // Without a use as 



    return 0;
}
```

## Considerations

It might be wise to allow subfolders in modules, despite the fact that the above example doesn't indicate that useage. 

# Syntax

## Variables 

    **str**  - A string, or a char. Some manner of text

    **int**  - Integer

    **real** - A 'real' number "double"

    **nil**  - Not anything

### Setting variables 

Use 'let' for creating new variables. Once created, let no longer required.

```
    let int a = 43;

    let str b = "This is a string!\n";

    let real c = 3.141559;
```

### Creating objects

```
    object my_obj
    {
        int some_int;
        str some_str;
    }

def example(){

    let my_obj o = { 42, "A str" };

    o.some_int = 54; 
}

```

### Copy / Delete

All things are passed 'by reference.' To pass a copy, a new copy must be made. 

```
    let int a = 32;

    let int b = copy(a);
```

Deletes happen if an item scope is left, but to delete an item before-hand :

```
    let int a = 55;

    delete(a);
```

### Globals

Globals are only global to the file they are contained in, but never 'leave' scope. 


```
global int my_global_int = 42;

def some_method() {

    // Really just a defined scope that can be reached 
    global.my_global_int = 55;
}

```

### Passing variables

```

def callee(int a, str c) {

    // a = 99
    // c = "Josh"
}

def main() {

    let int some_int = 99;
    let str name = "Josh";

    // Note: All things passed are references. Not copies 
    callee(some_int, name);
}



```

# Library Layout


```
Library Directory
    |
    |----modules
    |       |----- something.n
    |       |----- somethingelse.n
    |       |----- mod.n
    |
   lib.n
   config.json
```

Libraries are very similar to projects, with the exception of _lib.n_ and _config.json_.
With libraries the config should also state what dependencies it has, and install information. 
The _lib.n_ file is similar to _mod.n_ files in-that they extend the contained modules for access to applications that include the library. 


# Tests

In any file that exists if there is a block wrapped as such : 

```

test{

    // Some code

    assert(something)

}


```

Will be executed at soon as they are found either during compile time or a full project interpretation. 

# Control flow

```

def flow() -> nil{

    if ( a == b) {

    }
    elif (a == c) {

    }
    else {
        // ugh
    }

    for 0 to a step 1 {

    }

    loop.outer {

        loop.inner {

            loop.way_inner {

                break.outer;
            }
        }
    }


    match(a){

        is(4) {

        }

        is(5) {

        }

        default {

        }
    }

    // Match should always return the type specified by the type of the lhs operand. If it doesn't error.
    // If not used as such, and instead is used as match above, returns should always be nil or none at all.

    let int b = match (a) {

        is(4){ return 4*2; }

        is(5){ return 5*2; }

        default{ 1 }
    }

}


```

# Always present methods

```

    assert( condition )

    copy( var )

    delete( var )

    exit()

```

# Keywords

```
global
let
int
str
real
double
nil
for
in 
step
match
is
default
def
test
assert
copy
delete
exit
return
yield
if
elif
else
loop
use
as
public
fwd
```

# Ops

```
=
->
<
>
<=
>=
==
!
!= 
~
~=
/
/=
*
*=
**
**=
+
+=
-
-=
^
^=
%
<<
<<=
>>
>>=
```