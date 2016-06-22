/****************************************************/
/* File: cminus.l                                     */
/* Lex specification for CMINUS                       */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "y.tab.h"

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
char lastToke[MAXTOKENLEN+1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+
backslash   \\

%%
"else"          {return ELSE;}
"if"            {return IF;}
"int"           {return INT;}
"return"        {return RETURN;}
"void"          {return VOID;}
"while"         {return WHILE;}

"="             {return ASSIGN;}
"=="            {return EQ;}
"!="            {return NOTEQ;}
"<="            {return LTEQ;}
"<"             {return LT;}
">="            {return GTEQ;}
">"		{return GT;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return MULT;}
"/"             {return DIV;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"["             {return LINDEX;}
"]"             {return RINDEX;}
"{"             {return LBRACKET;}
"}"             {return RBRACKET;}  
";"             {return SEMI;}
","             {return COMMA;}
{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"             {   char c;
                    while (1){
                        c = input();
                        if (c == EOF) break;
                        if (c == '\n') lineno++;
                        if (c == '*') {
                            c = input();
                            if (c == '/' || c == EOF) break;
                            if (c == '\n') lineno++;
                        }
                    }
                }
.               {return ERROR;}

%%

int getToken(void)
{ static int firstTime = TRUE;
  int currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  strncpy(lastToke,tokenString,MAXTOKENLEN);
  currentToken = yylex();
  strncpy(tokenString, yytext, MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}
