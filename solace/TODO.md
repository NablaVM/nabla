
Currently arithmatic operations are being generated, and so are function start/stop/accumulate stuff


0   Next step is to read in labels, and mark where they are in the currentFunction struct. 
    Actual address should be the number of bytes in the current function / 8 as that will be the
    total number of instructions added at the time of the label being found. The label value
    should be that number so a branch can find where in the function to jump to. The actual
    label its self should not be encoded into the function instructions.

1   Once the labels are completed, branches can be implemented in the byte generator

2   Jump / Call

3   Move / push / pop / stb

4   ldb

5   lda 

6   Make .file do something usefuk

7   Write some unit tests

8   Leveraging the unit tests make things more pretty and ensure everything is still working

...

   Once all instructions have been verified, THEN the VM can start being impld