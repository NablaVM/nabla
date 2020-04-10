### Function definitions

// Open bracket must be on same line as function definition

// Param types need to either be prims or conform tp varaible names as they might
//  be a user-define item

def functionName( int:Name, real:othername ) -> int {


    emit 4

} // Parse needs to check that a definition has started


// Specifies that the function can be reached from anywhere. This might not be implemented at first
// it may be a later expansion

def pub::functionName( int:Name ) -> nil {


}


// 'Entry' method for program - Since vec is an expansion, first version will simply be without that 

def main(vec<str> arguments) -> nil {


}