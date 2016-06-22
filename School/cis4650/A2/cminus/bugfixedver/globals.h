/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for CMINUS compiler        */
/* must come before other include files             */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */
extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/
//StmtK,ExpK,DecK,
typedef enum {StmtK,ExpK,DecK,TypeK} NodeKind;
//typedef enum {IfK,RepeatK,AssignK,ReadK,WriteK} StmtKind;
//typedef enum {OpK,ConstK,IdK} ExpKind;
//typedef enum {VarK,FunK} DecKind;
//typedef enum {IdK, ArrayK} VarKind;
typedef enum {IfK, ReturnK, WhileK, CompK} StmtKind;
typedef enum {OpK, ConstK, IdK, CallK, SubsK, AssignK} ExpKind;
typedef enum {VarK, FunK, ParamK} DecKind;
typedef enum {VoidK,IntK} TypeKind;
//typedef enum {LteqK,LtK,GteqK,GtK,EqK,NoteqK) Relopk;
/* ExpType is used for type checking */
typedef enum {Void,Integer,Boolean} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
   { struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int lineno;
     NodeKind nodekind;
     union { ExpKind exp; StmtKind stmt; DecKind dec; TypeKind type;} kind;
     union { int op;
             int val;
             char * name; } attr;
     ExpType type; /* for type checking of exps */
   } TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

#endif
