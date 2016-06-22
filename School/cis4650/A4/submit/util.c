/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the CMINUS compiler                            */
/****************************************************/

#include "globals.h"
#include "hashtable.h"
#include "util.h"
#include "y.tab.h"

#define CODESIZE 100
#define pc 7
#define gp 6
#define fp 5
#define ac 0
#define ac1 1
#define ofpFO 0
#define retFO -1
#define initFO -2
extern Entry * table[MAXSIZE];
extern FILE * listing;

static int emitLoc = 0;
static int highEmitLoc = 0;
static int frameoffset = 0;
static int globaloffset = 0;

static void printNSpaces(int n)
{ int i;
  for (i=0;i<n;i++)
    fprintf(listing," ");
}

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
ExpType checkReturn(TreeNode * node){//, int level, int type){
    TreeNode * temp = node;
    while (temp != NULL){
        if (temp->nodekind == StmtK){
            if (temp->kind.stmt == ReturnK){
                TreeNode * ret = temp->child[0];
                if (ret == NULL){
                    return Void;
                }
                else {
                    return Integer;
                }
            }
            if (temp->kind.stmt == CompK){
                return checkReturn(temp->child[1]);
            }
            if (temp->kind.stmt == IfK){
                return checkReturn(temp->child[1]);
            }
        }
        temp = temp->sibling;
    }
    return None;
}

void checkReturns(TreeNode * node, ExpType type, int level){
    TreeNode * temp = node;
    ExpType found = None;
    while (temp != NULL){
        if (temp->nodekind == StmtK){
            if (temp->kind.stmt == ReturnK){
                found = checkReturn(temp);
                if (type == Integer && found == Void){
                    fprintf(stderr,"Line %d: Function must return a non-void value.\n",temp->lineno);
                }
            }
            if (temp->kind.stmt == CompK){
                found = checkReturn(temp->child[1]);
                if (type == Integer && found == Void){
                    fprintf(stderr,"Line %d: Function must return a non-void value\n",temp->lineno);
                }
            }
            if (temp->kind.stmt == IfK){
                found = checkReturn(temp->child[1]);
                if (type == Integer && found == Void){
                    fprintf(stderr,"Line %d: Function must return a non-void value\n",temp->lineno);
                }
            }
        }
        temp = temp->sibling;
    }
//     if (type == 1 && found == 0){
//         fprintf(stderr,"Line %d: Non-void function must have a return statement.\n",node->lineno);
//     }
}

//             if (type == 1){
//                 if (ret == NULL){
//                     fprintf(stderr,"Line %d: Function must return a value\n",ret->lineno);
//                 }
//                 else{
//                     if (typeOf(ret,level) == Void){
//                         fprintf(stderr,"Line %d: Function must return a non-void value.\n",ret->lineno);
//                     }
//                 }


//The main function used for type checking
void checkExps(TreeNode * exp, int level){
    TreeNode * temp = exp;
    while (temp != NULL){
        if (temp->nodekind == ExpK){
            if (temp->kind.exp == AssignK){
                Entry * e = lookup(temp->child[0]->attr.name,level);
                if (e != NULL && e->defn->kind.dec == ArrayK){
                    if (temp->child[0]->child[0] == NULL){
                        fprintf(stderr,"Line %d: Array variable %s must have subscript\n",temp->child[0]->lineno,temp->child[0]->attr.name);
                    }
                }
                if ((typeOf(temp->child[0],level)) == Integer &&
                    (typeOf(temp->child[1],level)) == Integer){
                        //genCode(temp,NULL);
                    }
                else if ((typeOf(temp->child[0],level)) == Integer &&
                        (typeOf(temp->child[1],level)) == Void){
                        fprintf(stderr,"Line %d: Integer variable %s cannot be assigned value of void.\n",temp->child[0]->lineno,temp->child[0]->attr.name);
                    }
                else if ((typeOf(temp->child[0],level)) == Void &&
                        (typeOf(temp->child[1],level)) == Integer){
                        fprintf(stderr,"Line %d: Void variable %s cannot be assigned value of integer.\n",temp->child[0]->lineno,temp->child[0]->attr.name);
                    }
            }
            if (temp->kind.exp == CallK){
                typeOf(temp,level);
            }
            if (temp->kind.exp == IdK){
                typeOf(temp,level);
            }
            if (temp->kind.exp == OpK){
                //genCode(temp,NULL);
            }
            //printf("%d\n",typeOf(temp,level));
        }
        if (temp->nodekind == StmtK){
            if (temp->kind.stmt == IfK){
                if (typeOf(temp->child[0],level) != Integer){
                    fprintf(stderr,"Line %d: Test condition must be an integer.\n",temp->child[0]->lineno);
                }
                printNSpaces((level+1)*4);
                fprintf(listing,"Entering If (then) Statement\n");
                BuildSymbolTable(temp->child[1],level+1);
                checkExps(temp->child[1],level+1);
                printTable(table,level+1);
                delDepth(level+1);
                if (temp->child[2] != NULL){
                    printNSpaces((level+1)*4);
                    fprintf(listing,"Entering If (else) Statement\n");
                    BuildSymbolTable(temp->child[2],level+1);
                    checkExps(temp->child[2],level+1);
                    printTable(table,level+1);
                    delDepth(level+1);
                }
            }
            if (temp->kind.stmt == CompK){
                printNSpaces((level+1)*4);
                fprintf(listing,"Entering Compound Statement\n");
                BuildSymbolTable(temp->child[0],level+1);
                checkExps(temp->child[1],level+1);
                printTable(table,level+1);
                delDepth(level+1);
            }
            if (temp->kind.stmt == WhileK){
                if (typeOf(temp->child[0],level) != Integer){
                    fprintf(stderr,"Line %d: Test condition must be an integer.\n",temp->child[0]->lineno);
                }
                printNSpaces((level+1)*4);
                fprintf(listing,"Entering While Statement\n");
                BuildSymbolTable(temp->child[1],level+1);
                checkExps(temp->child[1],level+1);
                printTable(table,level+1);
                delDepth(level+1);
            }
        }
        temp = temp->sibling;
    }
}

//Returns the type of an expression
ExpType typeOf(TreeNode * exp, int level){
    if (exp->nodekind == DecK){
        if (exp->kind.dec == ParamK){
            return exp->type;
        }
    }
    if (exp->kind.exp == IdK || exp->kind.exp == SubsK){
        int k = level;
        Entry * e;// = lookup(exp->attr.name, level);
        while (k >= 0 && ((e = lookup(exp->attr.name, k)) == NULL )){
            k--;
        }
        if (e == NULL){
            fprintf(stderr,"Line %d: Variable %s has not been defined.\n",exp->lineno, exp->attr.name);
            return Integer;
        }
        else{
            if (exp->kind.exp == SubsK &&
                typeOf(exp->child[0],level) != Integer){
                fprintf(stderr,"Line %d: Index of array must be an integer.\n",exp->lineno);
            }
            return e->defn->type;
        }
    }
    if (exp->kind.exp == ConstK){
        return Integer;
    }
    if (exp->kind.exp == OpK){
        if ((typeOf(exp->child[0],level)) == Integer &&
            (typeOf(exp->child[1],level)) == Integer){
                return Integer;
        }
        if ((typeOf(exp->child[0],level)) == Void &&
            (typeOf(exp->child[1],level)) == Void){
                return Void;
        }
        else {
            fprintf(stderr,"Line %d: Operands must be of the same type.\n",exp->lineno);
            return Integer;
        }
    }
    if (exp->kind.exp == CallK){
        Entry * e = lookup(exp->attr.name, 0);
        if (e == NULL){
            fprintf(stderr,"Line %d: Function %s has not been defined.\n",exp->lineno, exp->attr.name);
            return Void;
        }
        else{
            int i = 0;
            TreeNode * arg = exp->child[0];
            TreeNode * param = e->defn->child[1];
            if (param == NULL && arg != NULL){
                fprintf(stderr,"Line %d: Call to function %s contains too many arguments.\n",exp->lineno, e->name);
            }
            if (param != NULL && arg == NULL){
                fprintf(stderr,"Line %d: Call to function %s contains too few arguments.\n",exp->lineno, e->name);
            }
            while (param != NULL && arg != NULL){
                //printf("%d %d__\n",typeOf(param,0),typeOf(arg,level));
                if (typeOf(param,0) != typeOf(arg,level)){
                    fprintf(stderr,"Line %d: Arguments to %s do not match definition.\n",exp->lineno, e->name);
                }
                if (param->sibling == NULL && arg->sibling != NULL){
                    fprintf(stderr,"Line %d: Call to function %s contains too many arguments.\n",exp->lineno, e->name);
                }
                if (param->sibling != NULL && arg->sibling == NULL ){
                    fprintf(stderr,"Line %d: Call to function %s contains too few arguments.\n",exp->lineno, e->name);
                }
                arg = arg->sibling;
                param = param->sibling;
            }
            return e->defn->type;
        }
    }
}

int emitSkip(int dist){
    int i = emitLoc;
    emitLoc += dist;
    if (highEmitLoc < emitLoc)
        highEmitLoc = emitLoc;
    return i;
}

void emitBackup(int loc){
    emitLoc = loc;
}

void emitRestore(void){
    emitLoc = highEmitLoc;
}

void emitRM_Abs(char * op, int r, int a){
    fprintf(listing,"%3d: %5s %d, %d(%d)\n",emitLoc,op,r,a-(emitLoc+1),pc);
    emitLoc++;
    if (highEmitLoc < emitLoc)
        highEmitLoc = emitLoc;
}

void emitRO(char * op, int r, int s, int t){
    fprintf(listing,"%3d: %5s %d, %d, %d\n",emitLoc,op,r,s,t);
    emitLoc++;
    if (highEmitLoc < emitLoc)
        highEmitLoc = emitLoc;
}

void emitRM(char * op, int r, int d, int s){
    fprintf(listing,"%3d: %5s %d, %d(%d)\n",emitLoc,op,r,d,s);
    emitLoc++;
    if (highEmitLoc < emitLoc)
        highEmitLoc = emitLoc;
}


char * newTemp(void){
    static int t;
    char * ret = malloc(sizeof(char)*5);
    sprintf(ret,"t%d",t);
    t++;
    return ret;
}

char * newLabel(void){
    static int L;
    char * ret = malloc(sizeof(char)*5);
    sprintf(ret,"L%d",L);
    L++;
    return ret;
}

void genCompound(TreeNode * node, int level){
    BuildSymbolTable(node->child[0],level);
    TreeNode * temp = node->child[1];
    while (temp != NULL){
        genCode(temp,0,0);
        temp = temp->sibling;
    }
    delDepth(level+1);
}

void genDec(TreeNode * node, int offset){
    if (node == NULL) return;
    char codeStr[CODESIZE];
    switch (node->kind.stmt){
        case VarK:
            node->offset = offset;
            if (node->sibling != NULL){
                genDec(node->sibling, offset-1);
                frameoffset -= 1;
            }
            break;
        case FunK:
            frameoffset = initFO;
            genCode(node->child[1]->child[0],-2,0);
            genCode(node->child[1]->child[1],0,0);
            break;
    }
}

void genExp(TreeNode * node, int offset, int isAddr){
    if (node == NULL) return;
    char codeStr[CODESIZE];
    switch (node->kind.exp){
        char * num;
        case OpK:
            genCode(node->child[0],0,0);
            genCode(node->child[1],0,0);
            node->strval = newTemp();
            switch(node->attr.op){
                case PLUS:
                    sprintf(codeStr,"%s = %s + %s",node->strval,node->child[0]->strval,node->child[1]->strval);
                    printf("%s\n",codeStr);
                    break;
            }
            break;
        case AssignK:
            ;
            TreeNode * e = lookup(node->child[0]->attr.name,1)->defn;
            sprintf(codeStr,"%s = %s",node->strval,node->child[1]->strval);
            printf("%s\n",codeStr);
            break;
        case IdK:
            break;
        case ConstK:
            break;
    }
}

void genStmt(TreeNode * node, int offset){
    if (node == NULL) return;
    char codeStr[CODESIZE];
    switch(node->kind.stmt){
        case CompK:
            //genCompound(node,0);
            break;
        case IfK:
            genCode(node->child[0],0,0);
            char * lab1 = newLabel();
            char * lab2 = NULL;
            if (node->child[0] == 0)
                sprintf(codeStr,"if_false false goto %s",lab1);
            else
                sprintf(codeStr,"if_false true goto %s",lab1);
            printf("%s\n",codeStr);
            genCode(node->child[1],0,0);
            if (node->child[2] != NULL){
                lab2 = newLabel();
                sprintf(codeStr,"goto %s",lab2);
                printf("%s\n",codeStr);
            }
            sprintf(codeStr,"label %s",lab1);
            printf("%s\n",codeStr);
            if (node->child[2] != NULL){
                genCode(node->child[2],0,0);
                sprintf(codeStr,"label %s",lab2);
                printf("%s\n",codeStr);
            }
            break;
    }
}

void genCode(TreeNode * node,int offset, int isAddr){
    if (node != NULL){
        switch (node->nodekind){
            case StmtK:
                genStmt(node, offset);
                break;
            case ExpK:
                genExp(node,offset,isAddr);
                break;
            case DecK:
                genDec(node,offset);
                break;
        }
    }
}

void genC(TreeNode * node){
    
    //Generate prelude
    emitRM("LD",gp,0,0);
    emitRM("LDA",fp,0,gp);
    emitRM("ST",0,0,0);
    
    //gen IO routines
    
    int savedLoc = emitSkip(1);
    //input
    emitRM("ST",ac,-1,fp);
    emitRO("IN",0,0,0);
    emitRM("LD",pc,-1,fp);
    
    //output
    emitRM("ST",ac,-1,fp);
    emitRM("LD",ac,-2,fp);
    emitRO("OUT",0,0,0);
    emitRM("LDA",pc,-1,fp);
    
    //jump around IO code
    emitBackup(savedLoc);
    emitRM("LDA",pc,7,pc);
    emitRestore();
    
    //generate code for program
    //genCode(node,0,0);
    
    //generate finale
    emitRM( "ST", fp, globaloffset+ofpFO, fp);
    emitRM( "LDA", fp, globaloffset, fp);
    emitRM( "LDA", ac, 1, pc);
    emitRM_Abs( "LDA", pc, 0);
    emitRM( "LD", fp, ofpFO, fp);
    emitRM( "HALT", 0, 0, 0);
    
}
    


void Code(TreeNode * node, int level){
    TreeNode * temp = node;
    while(temp != NULL){
        if (temp->nodekind == DecK){
            if (temp->kind.dec == VarK){
                insert(temp->attr.name,level,temp);
            }
            else if (temp->kind.dec == ArrayK){
                insert(temp->attr.name,level,temp);
            }
            else if (temp->kind.dec == FunK){
                insert(temp->attr.name,level,temp);
                TreeNode * params = temp->child[1];
                while (params != NULL){
                    insert(params->attr.name, level+1, params);
                    params = params->sibling;
                }
                genCompound(temp->child[2],level+1);
            }
        }
        if (temp->nodekind == StmtK){
            if (temp->kind.stmt == CompK){
                genCompound(temp->child[0],level+1);
                //BuildSymbolTable(temp->child[0],level+1);
                //delDepth(level+1);
            }
        }
        temp = temp->sibling;
    }
}


//Creates the symbol table at a certain depth
void BuildSymbolTable(TreeNode * node,int level){
    TreeNode * temp = node;
    if (level == 0){
        fprintf(listing,"Entering Global Scope\n");
    }
    while (temp != NULL){
        if (temp->nodekind == DecK){
            if (temp->kind.dec == VarK){
                if (lookup(temp->attr.name,level) == NULL){
                    insert(temp->attr.name,level,temp);
                }
                else{
                    fprintf(stderr,"Line %d: Variable %s is already defined.\n",temp->lineno, temp->attr.name);
                }
            }
            else if (temp->kind.dec == ArrayK){
                if (lookup(temp->attr.name,level) == NULL){
                    insert(temp->attr.name,level,temp);
                }
                else{
                    fprintf(stderr,"Line %d: Variable %s is already defined.\n",temp->lineno, temp->attr.name);
                }
            }
            else if (temp->kind.dec == FunK){
                Entry * e;
                if ((e = lookup(temp->attr.name,level)) != NULL && e->defn->kind.dec == FunK){
                    fprintf(stderr,"Line %d: Function %s is already defined.\n",temp->lineno, temp->attr.name);
                }
                insert(temp->attr.name,level,temp);
                printNSpaces((level+1)*4);
                fprintf(listing,"Entering function %s\n",temp->attr.name);
                TreeNode * params = temp->child[1];
                while (params != NULL){
                    insert(params->attr.name, level+1, params);
                    params = params->sibling;
                }
                BuildSymbolTable(temp->child[2]->child[0],level+1);
                checkExps(temp->child[2]->child[1],level+1);
                //checkReturns(temp->child[2]->child[1],temp->type,level+1);
                printTable(table,level+1);
                delDepth(level+1);
            }
        }
        temp = temp->sibling;
    }
    if (level == 0){
        printTable(table,level);
        delDepth(level);
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
        case ArrayK:
          fprintf(listing,"Declare Array Var: %s\n", tree->attr.name);
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
