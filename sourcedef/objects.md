### Objects

This is a future expansion, that won't be in first work-up and may be subject to change

Object fields are public by default, but objects themselves aren't

object MyObject {

    def construct( int:someInt ) {

        // Construct has no emit (return)
    }

    def destruct() {            // No params

        // Destruct has no emit (return)
    }

    priv {

        int: privateInt;

        // def pub::name is not allowed here. 
        def somePrivateFunction( int:param ) -> nil {


        }


    }

}