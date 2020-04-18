# TODO

Need to make a storage container for functions. Functions five us the num instructions within them, so it wont need to change size or anything.
We'll just need a place to store them and a means to access by index. 

Thinking of it.. registers are like.. 8 bytes. The current stack object is built for 1 byte. We'll need to change that to use uint64s instead of uint8