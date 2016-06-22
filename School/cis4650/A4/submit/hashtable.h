#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"

#define MAXSIZE 97
#define SHIFT 4

typedef enum {INT_e, VOID_e, FUNC_e} Entry_t;

typedef struct entry {
    char * name;
    int depth;
    TreeNode * defn;
    struct entry * next;
} Entry;

void printEntry(Entry * e);
void printTable(Entry ** table,int level);
void initTable(Entry ** table);
Entry * newEntry(char * c, int depth, TreeNode * def);
int insert (char * c, int depth, TreeNode * def);
void delDepth(int level);
Entry * lookup(char * name,int level);