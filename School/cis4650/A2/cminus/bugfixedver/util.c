/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the CMINUS compiler                            */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "y.tab.h"

/* prints a token and its lexeme to the listing file
 */
void printToken( int token, const char* tokenString )
{ switch (token)
  { case IF:
    case ELSE:
    case INT:
    case RETURN:
    case WHILE:
    case VOID:
      fprintf(listing,
         "reserved word: %s\n",tokenString);
      break;
    case ASSIGN: fprintf(listing,"=\n"); break;
    case LT: fprintf(listing,"<\n"); break;
    case GT: fprintf(listing,">\n"); break;
    case EQ: fprintf(listing,"==\n"); break;
    case NOTEQ: fprintf(listing,"!=\n"); break;
    case GTEQ: fprintf(listing,">=\n"); break;
    case LTEQ: fprintf(listing,"<=\n"); break;
    case LPAREN: fprintf(listing,"(\n"); break;
    case RPAREN: fprintf(listing,")\n"); break;
    case LINDEX: fprintf(listing,"[\n"); break;
    case RINDEX: fprintf(listing,"]\n"); break;
    case LBRACKET: fprintf(listing,"{\n"); break;
    case RBRACKET: fprintf(listing,"}\n"); break;
    case SEMI: fprintf(listing,";\n"); break;
    case COMMA: fprintf(listing,",\n"); break;
    case PLUS: fprintf(listing,"+\n"); break;
    case MINUS: fprintf(listing,"-\n"); break;
    case MULT: fprintf(listing,"*\n"); break;
    case DIV: fprintf(listing,"/\n"); break;
    case 0: fprintf(listing,"EOF\n"); break;
    case NUM:
      fprintf(listing,
          "NUM, val= %s\n",tokenString);
      break;
    case ID:
      fprintf(listing,
          "ID, name= %s\n",tokenString);
      break;
    case ERROR:
      fprintf(listing,
          "ERROR: %s\n",tokenString);
      break;
    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
  }
}

void printTokenERROR( int token, const char* tokenString )
{ switch (token)
  { case IF:
    case ELSE:
    case INT:
    case RETURN:
    case WHILE:
    case VOID:
      fprintf(stderr,
         "reserved word: %s\n",tokenString);
      break;
    case ASSIGN: fprintf(stderr,"=\n"); break;
    case LT: fprintf(stderr,"<\n"); break;
    case GT: fprintf(stderr,">\n"); break;
    case EQ: fprintf(stderr,"==\n"); break;
    case NOTEQ: fprintf(stderr,"!=\n"); break;
    case GTEQ: fprintf(stderr,">=\n"); break;
    case LTEQ: fprintf(stderr,"<=\n"); break;
    case LPAREN: fprintf(stderr,"(\n"); break;
    case RPAREN: fprintf(stderr,")\n"); break;
    case LINDEX: fprintf(stderr,"[\n"); break;
    case RINDEX: fprintf(stderr,"]\n"); break;
    case LBRACKET: fprintf(stderr,"{\n"); break;
    case RBRACKET: fprintf(stderr,"}\n"); break;
    case SEMI: fprintf(stderr,";\n"); break;
    case COMMA: fprintf(stderr,",\n"); break;
    case PLUS: fprintf(stderr,"+\n"); break;
    case MINUS: fprintf(stderr,"-\n"); break;
    case MULT: fprintf(stderr,"*\n"); break;
    case DIV: fprintf(stderr,"/\n"); break;
    case 0: fprintf(stderr,"EOF\n"); break;
    case NUM:
      fprintf(stderr,
          "NUM, val= %s\n",tokenString);
      break;
    case ID:
      fprintf(stderr,
          "ID, name= %s\n",tokenString);
      break;
    case ERROR:
      fprintf(stderr,
          "ERROR: %s\n",tokenString);
      break;
    default: /* should never happen */
      fprintf(stderr,"Unknown token: %d\n",token);
  }
}

/* creates a new statement node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

TreeNode * newTypeNode(TypeKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = TypeK;
    t->kind.type = kind;
    t->lineno = lineno;
  }
  return t;
}

TreeNode * newDecNode(DecKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = DecK;
    t->kind.dec = kind;
    t->lineno = lineno;
  }
  return t;
}


/* newExpNode creates a new expression node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* allocates and makes a new copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

/* used by printTree to store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

/* prints a syntax tree to the listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree )
{ int i;
  INDENT;
  //printf("k\n");
  while (tree != NULL) {
    printSpaces();
    //printf("i\n");
    if (tree->nodekind==StmtK)
    { switch (tree->kind.stmt) {
        case IfK:
          fprintf(listing,"If\n");
          break;
        case ReturnK:
          fprintf(listing,"Return\n");
          break;
        case WhileK:
          fprintf(listing,"While\n");
          break;
        case CompK:
          fprintf(listing,"Compound Stmt:\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK)
    { switch (tree->kind.exp) {
        case OpK:
          fprintf(listing,"Op: ");
          printToken(tree->attr.op,"\0");
          break;
        case ConstK:
          fprintf(listing,"Const: %d\n",tree->attr.val);
          break;
        case IdK:
          fprintf(listing,"Id: %s\n",tree->attr.name);
          break;
        case CallK:
          fprintf(listing,"Call: %s\n",tree->attr.name);
          break;
        case SubsK:
          fprintf(listing,"Array ID: %s\n",tree->attr.name);
          break;
        case AssignK:
          fprintf(listing,"Assign\n");
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==DecK)
    { switch (tree->kind.dec) {
        case VarK:
          fprintf(listing,"Declare Var: %s\n", tree->attr.name);
          break;
        case FunK:
          fprintf(listing,"Declare Func: %s\n",tree->attr.name);
          break;
        case ParamK:
          fprintf(listing,"Param: %s\n",tree->attr.name);
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
      else if (tree->nodekind==TypeK)
    { switch (tree->kind.type) {
        case IntK:
          fprintf(listing,"Type: int\n");
          break;
        case VoidK:
          fprintf(listing,"Type: void\n");
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else fprintf(listing,"Unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++)
         printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}
