
## Later

Update asm_instructions - There should be a nice table of ALL instructions, and their hex representation.
A showing of who uses id bits, etc

Update language documentation. Explain how the binary file should be loaded. Explain some of the instructions that aren't used by people, but are used by the loader, etc

< constant seg start instruct >
< constant starts >
.int8 
.int8
..
..
.string
..
< constant end >
< function seg start instruct >
< function start>
ins
ins
..
ins
..
< function end >
< function start>
ins
ins
..
ins
..
< function end >
< function start>
ins
ins
..
ins
..
< function end >

< binary eof inst >
# Much later

After _EVERYTHING_ above is done and **all** instructions in existence are tested in vm tests, then more instructions can be introduced. Primarily we need to be able to grab specific bytes out of registers, and be able to shift values that are in registers

Think about these:
    rmod r0 r1 (ub | umb | lmb | lb) ; get upper byte, upper middle, lower middle, lower byte of r1 and store it in r0

## External modifications

Make some sort of interface to the vm so other programs can get information about whats going on
access registers, read global stack, etc

It might be neat to add built-in libraries to handle stdio etc that can be side-loaded. As-in communicates with user functions via registers and global stack, but not directly invokable. If they were, it would fuck with how function addresses are determined by solace. If that could be totally separated so functionality could be added / removes  without a total recompile that would be cool. 
    This might be able to be done with 'interrupts' or 'signals' -- WAY DOWN THE LINE

User adds 'signal handlers' functions, and 'emitter' instructions so they can interract with modules without knowing where they are, and ensuring that a change in the mod doesn't demand a recompile of the main program.

