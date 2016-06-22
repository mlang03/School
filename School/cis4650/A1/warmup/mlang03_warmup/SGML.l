/* Flex specification for SGML language
 * Author: Michael Lang
 * Edited from file given by Fei Song
 * Feb 4th 2016
*/

%{
#include "globals.h"

/* lexeme of identifier or reserved word */
char token[token_max_length+1];
int numOpen = 0;
int numClose = 0;
extern FILE * input_file;
extern FILE * output_file;
%}

digit           [0-9]
letter          [a-zA-Z]
identifier      {letter}+
newline         \n
whitespace      [ \t]+
opentag         "<"[a-zA-Z0-9 "=-_!?:;']+">"
closetag        "<""/"[a-zA-Z0-9 "=-_!?:;']+">"
word            [0-9]*[a-zA-Z][a-zA-Z0-9]*
number          -?([0-9]+"."[0-9]+|"."[0-9]+|[0-9]+)
hyphenated      [a-zA-Z0-9]+-[a-zA-Z0-9]+(-[a-zA-Z0-9]+)?
apostrophized   [0-9]*[a-zA-Z][a-zA-Z0-9]*('[a-zA-Z0-9]+){1,2}|[0-9]*[a-zA-Z][a-zA-Z0-9]*-[a-zA-Z0-9]+'[a-zA-Z0-9]+|[0-9]*[a-zA-Z][a-zA-Z0-9]*-[a-zA-Z0-9]+-[a-zA-Z0-9]+'[a-zA-Z0-9]+
%%

{opentag}           {numOpen++; return OPEN_TAG;}
{closetag}          {numClose++; return CLOSE_TAG;}
{apostrophized}     {return APOSTROPHIZED;}
{hyphenated}        {return HYPHENATED;}
{number}            {return NUMBER;}
{word}              {return WORD;}
{whitespace}        {}
{newline}           {}
.                   {return PUNCTUATION;}
%%


Token_type getToken(void){
    static int firstTime = 1;
    if (firstTime){
        firstTime = 0;
        yyin = input_file;
        yyout = output_file;
    }
    Token_type current = yylex();
    strncpy(token,yytext,token_max_length);
    return current;
}