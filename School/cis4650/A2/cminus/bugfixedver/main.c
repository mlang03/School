/****************************************************/
/* File: main.c                                     */
/* Main program for CMINUS compiler                   */
/****************************************************/

#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#endif

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int main( int argc, char * argv[] )
{ TreeNode * syntaxTree;
  int ttype;
  char pgm[120]; /* source code file name */
  char outFILE[120];
  if (argc != 3)
    { fprintf(stderr,"usage: %s <filename> <option>\n",argv[0]);
      exit(1);
    }
  strcpy(pgm,argv[1]);
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".cm");
  if (strchr (pgm, '.') != NULL){
      int len = strlen(pgm) - 1;
      char c;
      while ((c = pgm[len--] != '.'))
      strncpy(outFILE,pgm,len);
      strcat(outFILE,"abs");
  }
  source = fopen(pgm,"r");
  if (source==NULL)
  { fprintf(stderr,"File %s not found\n",pgm);
    exit(1);
  }
  listing = fopen(outFILE,"w"); /* send listing to screen */
#if NO_PARSE
  while( (ttype=getToken())!= 0 )
    printToken( ttype, tokenString );
#else
  syntaxTree = parse();
  if (TraceParse) {
    fprintf(listing,"Syntax tree:\n");
    printTree(syntaxTree);
  }
#endif
  fclose(source);
  return 0;
}

