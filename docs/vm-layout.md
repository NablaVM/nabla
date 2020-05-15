# VM Layout

Eventually a class diagram would be nice, but for now, this textual representation will do. 

Each VM comes from a VSysMachine. This machine is the Base VM that contains *global memory*, *function definitions*, *execution contexts*, *external devices map*, and *standard devices*. There is more to a VSysMachine, but this is what really matters. 

Global memory is a VSysMemory object, the function definitions are vectors of bytes that hold the function instructions, execution contexts are vectors of VSysExectuionContext objects, and the external device map is a map  of device ids to VSysExternalIf. The standard devices are ExternalIfs that enable IO, Networking, and Host operations. 

The VSysExecutionContext objects are what actually execute code. A context takes in pointers to global memory and functions, and wraps functions with information used for execution (instruction pointer, local memory (another VSysMemory object) and a pointer into the actual function instructions.)

# VM Types

Each specific VM in use inherits a VSysMachine and performs their specific operation on and around the machine. For instance, we have a VSysLoadableMachine which takes care of loading binary files from disk and gets a VM ready to run. 
 