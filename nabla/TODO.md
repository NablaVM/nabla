# TODO

Create something to hold functions for vm. Everything is parsed from file up-to that point. 

Once something is in place to hold onto the instructions and a nice interface for the vm to interact
with the it (maybe make it a simple SIMPLE structure built-into the VM and not external to it) (but maybe not) then the fun part of executing instructions can begin.

The vm-loading code needs to be put somewhere.. like a vm-loader maybe? The only 'issue' is then some functionality of the vm that doesn't need to be exposed would be exposed (accessing global stack, etc)
--Though it might not be a bad idea. Exposing direct access to the stack would make later expansions easier?

Consider adding bytes at the top of the file to indicate how much data is constants so they can
be more explicitly loaded i.e load that whole chunk THEN start looking for function start, rather than
check opcodes and byte to see if its a constant or start of function. This will mean updating solace as-well-as the loader code. - I suspect we might get weird behaviour in edgecases otherwise

--Remove '"' from string constants in solace... i thought i did that, but apparently not... 