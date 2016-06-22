/****************************************************/
/* File: main.c                                     */
/* Main program for CMINUS compiler                   */
/****************************************************/

#include "globals.h"
#include "hashtable.h"
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
Entry * table[MAXSIZE];

int main( int argc, char * argv[] )
{ TreeNode * syntaxTree;
  int ttype;
  char pgm[120]; /* source code file name */
  char outFILE[120];
  int mode = -1;
  if (argc != 3)
    { fprintf(stderr,"usage: %s <filename> <option>\n",argv[0]);
      exit(1);
    }
  strcpy(pgm,argv[1]);
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".cm");
  if (strchr (pgm, '.') != NULL){
      int len = strlen(pgm) - 2;
      strncpy(outFILE,pgm,len);
  }
    if (strcmp(argv[2],"-s") == 0){
        mode = 2;
        strcat(outFILE,"sym");
        listing = fopen(outFILE,"w"); /* send listing to screen */
    }
    else if (strcmp(argv[2],"-a") == 0){
        mode = 1;
        strcat(outFILE,"abs");
        listing = fopen(outFILE,"w"); /* send listing to screen */
    }
  source = fopen(pgm,"r");
  if (source==NULL)
  { fprintf(stderr,"File %s not found\n",pgm);
    exit(1);
  }
#if NO_PARSE
  while( (ttype=getToken())!= 0 )
    printToken( ttype, tokenString );
#else
  syntaxTree = parse();
  if (mode == 1) {
    fprintf(listing,"Syntax tree:\n");
    printTree(syntaxTree);
  }
  else if (mode == 2){
    initTable(table);
    BuildSymbolTable(syntaxTree,0);
    }
#endif
  fclose(source);
  return 0;
}

