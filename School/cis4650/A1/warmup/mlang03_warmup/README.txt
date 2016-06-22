Compilers Warmup Assignment
By: Michael Lang
mlang03@mail.uoguelph.ca
Instructor: Fei Song
Feb 4th 2016

This Program creates a scanner for SGML files. It takes an SGML file as input and outputs
a set of tokens. Furthermore, tags which are deemed irrelevant are not included
in the output. The scanner also handles basic error checking such as when tags are not 
embedded properly or if there are unmatched tags at the end of input. As far as I know the
program works exactly to specifications given, although there may be minor errors due to 
ambiguity in the specification.

This program was tested using the "newsdata.txt" file that was given as well as with self made 
input files that intentionally contained errors, in order to test the error handling portion of
the assignment.

USAGE:

    Type "make" to compile the program.
    Type ./scanner < <inputfile.txt> to run.
    
