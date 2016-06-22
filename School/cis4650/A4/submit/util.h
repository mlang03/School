/****************************************************/
/* File: util.h                                     */
/* Utility functions for the CMINUS compiler          */
/****************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

/* prints a token and its value to the listing file
 */
void printToken( int, const char* );
void printTokenERROR( int, const char* );
void genC(TreeNode * node);
void Code(TreeNode * node, int level);
void genCode(TreeNode * node, int offset, int isAddr);
void BuildSymbolTable(TreeNode * tree, int level);
ExpType typeOf(TreeNode * exp, int level);

/* creates a new statement node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind);

/* creates a new expression node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind);

TreeNode * newDecNode(DecKind);

TreeNode * newTypeNode(TypeKind);

/* allocates and makes a new copy of an existing string
 */
char * copyString( char * );

/* prints a syntax tree to the * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * );

#endif
