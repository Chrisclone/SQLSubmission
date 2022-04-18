Hello, I'm Christopher Pestano and this is my submission for the COP4720 project.

HOW TO COMPILE

Linux

#1 If your computer has g++ just write make in the terminal while inside the directory, this generates the rangeQ executable,
this was tested on an azure ubuntu vm since I couldn't get the uf server ssh to work.

HOW TO USE

The program is very particular about how the command line args are inputted, here are the requirements.

ARG 1 [Index Type]: Must be 0,1, or 2 with {0: Sequential, 1: kd-tree, 2: mykd-tree}

ARG 2 [database]: Must be a single word file name without file extension i.e. like projDB not projDB.txt

ARG 3 [query]: Must also be a file, can have file extension but must be in the same directory as executable i.e. text2.txt not ./test/text2.txt

The above are intentional so that the output file is properly named according to the database, index, and query file used

POTENTIAL ISSUES

Since the database file given only had two dimensions, i.e. 0,0 1,2 ..., the program was designed to work with such files. It will likely not function
if there are additional dimensions.