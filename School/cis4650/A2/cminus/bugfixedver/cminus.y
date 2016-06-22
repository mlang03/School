/****************************************************/
/* File: cminus.y                                     */
/* The CMINUS Yacc/Bison specification file           */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedInt;
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static char * var_or_func_Name;
extern int yychar;

int yyerror(char * message)
{ fprintf(stderr,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(stderr,"Current token: ");
  printTokenERROR(yychar,tokenString);
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void) { 
  yyparse();
  return savedTree;
}

%}

%token ELSE IF INT RETURN VOID WHILE
%token ID NUM
%token PLUS MINUS MULT DIV LT LTEQ GT GTEQ EQ NOTEQ ASSIGN SEMI COMMA LPAREN RPAREN LINDEX RINDEX LBRACKET RBRACKET LCOM RCOM
%token ERROR 
 
%nonassoc EQ LT GT GTEQ LTEQ
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%left PLUS MINUS
%left MULT DIV

%% /* Grammar for C minus */

program         : declaration_list
                    {savedTree = $1;}
                ;

declaration_list    : declaration_list declaration
                        {YYSTYPE t = $1;
                        if (t != NULL){
                            while(t->sibling != NULL){
                                t = t->sibling;
                            }
                        t->sibling = $2;
                        $$ = $1;
                        } else {
                        $$ = $2;
                        }
                        }
                    | declaration
                        {$$ = $1;}
                    ; 

declaration     : fun_declaration
                    {$$ = $1;}
                | var_declaration
                    {$$ = $1;}
                | error 
                    {$$ = NULL;}
                | error SEMI
                    {$$ = NULL;}
                /*| error declaration
                    {$$ = NULL;}*/
                ;
                
var_or_func_start  : type_specifier ID 
                        {
                        var_or_func_Name = copyString(tokenString);
                        savedLineNo = lineno;
                        $$ = $1;
                        }
                    ;
fun_declaration : var_or_func_start LPAREN 
                    {
                    $$ = newDecNode(FunK);
                    $$->attr.name = var_or_func_Name;
                    $$->lineno = savedLineNo;
                    $$->child[0] = $1;
                    } params RPAREN compound_stmt
                    {$$ = $3;
                    $$->child[1] = $4;
                    $$->child[2] = $6;
                    }
                ;
                
var_declaration : var_or_func_start SEMI
                    {
                    $$ = newDecNode(VarK);
                    $$->attr.name = var_or_func_Name;
                    $$->child[0] = $1;
                    }
                | var_or_func_start LINDEX NUM
                    {
                    savedInt = atoi(tokenString);
                    }
                 RINDEX SEMI
                    {
                    $$ = newDecNode(VarK);
                    $$->attr.name = var_or_func_Name;
                    $$->child[0] = $1;
                    $$->child[1] = newExpNode(ConstK);
                    $$->child[1]->attr.val = savedInt;
                    }
                ;

type_specifier  : INT
                    {$$ = newTypeNode(IntK);}
                | VOID
                    {$$ = newTypeNode(VoidK);}
                ;


params          : param_list
                    {$$ = $1;}
                | VOID
                    {$$ = NULL;}
                | error
                    {$$ = NULL;}
                ;

param_list      : param_list COMMA param
                    {YYSTYPE t = $1;
                    if (t != NULL){
                        while(t->sibling != NULL){
                            t = t->sibling;
                        }
                    t->sibling = $3;
                    $$ = $1;
                    } else {
                    $$ = $3;
                    }
                    }
                | param
                    {$$ = $1;}
                ;
                
param_start     : type_specifier ID
                    {
                    $$ = newDecNode(ParamK);
                    $$->attr.name = copyString(tokenString);
                    $$->lineno = savedLineNo;
                    $$->child[0] = $1;
                    }
                ;

param           : param_start
                | param_start LINDEX RINDEX
    /*                    {
                        $$ = newDecNode(ParamK);
                        $$->attr.name = copyString(savedName);
                        }*/
                ;
                
args            : arg_list
                    {$$ = $1;}
                | /* empty */
                    {$$ = NULL;}
                ;
                
arg_list        : arg_list COMMA expression
                    {
                    YYSTYPE t = $1;
                    if (t != NULL){
                        while(t->sibling != NULL){
                            t = t->sibling;
                        }
                    t->sibling = $3;
                    $$ = $1;
                    } else {
                        $$ = $3;
                    }
                    }
                | expression
                    {$$ = $1;};
                ;
                
                
local_declarations  : local_declarations var_declaration
                        {YYSTYPE t = $1;
                        if (t != NULL){
                            while(t->sibling != NULL){
                                t = t->sibling;
                            }
                        t->sibling = $2;
                        $$ = $1;
                        } else {
                        $$ = $2;
                        }
                        }
                    | /* empty */
                        {$$ = NULL;}
                    | local_declarations error
                        {$$ = NULL;}
                    | error
                        {$$ = NULL;}
                    ;
                    
statement_list  : statement_list statement
                    {YYSTYPE t = $1;
                    if (t != NULL){
                        while(t->sibling != NULL){
                            t = t->sibling;
                        }
                    t->sibling = $2;
                    $$ = $1;
                    } else {
                    $$ = $2;
                    }
                    }
                | /* empty */
                    {$$ = NULL;}
                | statement_list error
                    {$$ = $1;}
                ;

statement       : expression_stmt
                    {$$ = $1;}
                | compound_stmt
                    {$$ = $1;}
                | selection_stmt
                    {$$ = $1;}
                | iteration_stmt
                    {$$ = $1;}
                | return_stmt
                    {$$ = $1;}
                | error
                    {$$ = NULL;}
                | error SEMI
                    {$$ = NULL;}
                | error '\n'
                    {$$ = NULL;}
                ;
                
expression_stmt : expression SEMI
                    {$$ = $1;}
                | SEMI
                    {$$ = NULL;}
                ;
                
compound_stmt   : LBRACKET local_declarations statement_list RBRACKET
                    {
                    $$ = newStmtNode(CompK);
                    $$->child[0] = $2;
                    $$->child[1] = $3;
                    }
                | LBRACKET error RBRACKET
                    {$$ = NULL;}
                ;
                
selection_stmt  : IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
                    {
                    $$ = newStmtNode(IfK);
                    $$->child[0] = $3;
                    $$->child[1] = $5;
                    }
                | IF LPAREN expression RPAREN statement ELSE statement
                    {
                    $$ = newStmtNode(IfK);
                    $$->child[0] = $3;
                    $$->child[1] = $5;
                    $$->child[2] = $7;
                    }
                ;

iteration_stmt  : WHILE LPAREN expression RPAREN statement
                    {$$ = newStmtNode(WhileK);
                    $$->child[0] = $3;
                    $$->child[1] = $5;
                    }
                ;
                
return_stmt     : RETURN SEMI
                    
                    {$$ = newStmtNode(ReturnK);}
                | RETURN expression SEMI
                    {
                    $$ = newStmtNode(ReturnK);
                    $$->child[0] = $2;
                    }
                ;
        /*moved call here*/        
call            : id LPAREN
                    {
                    $$ = newExpNode(CallK);
                    $$->attr.name = savedName;
                    $$->lineno = savedLineNo;
                    }
                args RPAREN
                    {
                    $$ = $3;
                    $$->child[0] = $4;
                    }
                ;
    
expression      : var ASSIGN expression
                    {
                    $$ = newExpNode(AssignK);
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                    }
                | expression op expression
                    {
                    $$ = newExpNode(OpK);
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                    $$->attr.op = $2;
                    }
                | LPAREN expression RPAREN
                    {$$ = $2;}
                | var
                    {$$ = $1;}
                | call
                    {$$ = $1;}
                | NUM
                    {
                    $$ = newExpNode(ConstK);
                    $$->attr.val = atoi(tokenString);
                    }
                | error
                    {$$ = NULL;}
                | error op error
                    {$$ = NULL;}
                | error SEMI
                    {$$ = NULL;}
                | LPAREN error RPAREN
                    {$$ = NULL;}
                ;
                
id              : ID
                {
                savedName = copyString(tokenString);
                savedLineNo = lineno;
                }
                ;

var             : id
                    {
                    $$ = newExpNode(IdK);
                    $$->attr.name = savedName;
                    $$->lineno = savedLineNo;
                    }
                | id LINDEX 
                {
                    $$ = newExpNode(SubsK);
                    $$->attr.name = savedName;
                    $$->lineno = savedLineNo;
                }
                expression RINDEX
                    {
                    $$ = $3;
                    $$->child[0] = $4;
                    }
                ;
                                
op              : LTEQ
                    {$$ = LTEQ;}
                | LT
                    {$$ = LT;}
                | GT
                    {$$ = GT;}
                | GTEQ
                    {$$ = GTEQ;}
                | EQ
                    {$$ = EQ;}
                | NOTEQ
                    {$$ = NOTEQ;}
                | PLUS
                    {$$ = PLUS;}
                | MINUS
                    {$$ = MINUS;}
                | MULT
                    {$$ = MULT;}
                | DIV
                    {$$ = DIV;}
                ;
                         
