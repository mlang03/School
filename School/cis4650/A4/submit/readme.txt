CMINUS COMPILER
Checkpoint 3, April 6th 2016
BY: Mike Lang

The program can be compiled with the “make” command. “make clean” will remove the
executable and any temporary files that were created in the process of compilation.

The program can be run with the command “./cm  <inputfile> <option>” where <inputfile> is
a cminus program, and <option> is either "-a" or "-s".

The command -a indicates that the program will produce an abstract syntax tree
representing the structure of the source program.

The command -s indicates that the program will produce a symbol table reprsenting the
variables and functions declared within the program.

The command -c indicates that the program will output tiny machine assembly code.

The abstract syntax tree will be written to a file called “inputfile.abs” where
inputfile is the name of the file containing the source program.

The symbol table will be written to a file called "inputfile.sym" where inputfile
is the name of the file containing the source program.

The tiny machine assembly code will be written to a file called "inputfile.tm" where inputfile
is the name of the file containing the source program.

MISSING FUNCTIONALITY:
	Most of code generation.