# Binary file structure


The structure for a binary file used with the LoadableMachine object should conform to the following format : 


**< seg const instruction >**

    [ bytes representing constants ]

**< seg func instruction >**

    [
        <function start>
        [ function instructions ]
        <function end>

        ...
        ...
        ...

        <function start>
        [ function instructions ]
        <function end>
    ]

**< binary end instruction >**