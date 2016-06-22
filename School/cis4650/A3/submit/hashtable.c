/* THIS FILE CONTAINS A HASHMAP IMPLEMENTATION */

#include "hashtable.h"

extern Entry * table[MAXSIZE];

void printEntry(Entry * e){
    int i;
    for (i = 0; i < (e->depth+1)*4; i++){
        fprintf(listing," ");
    }
    if (e->defn->nodekind == DecK){
        if (e->defn->kind.dec == VarK || e->defn->kind.dec == ParamK){
            if (e->defn->type == Integer)
                fprintf(listing,"%s: int var \n", e->name);
            else if (e->defn->type == Void)
                fprintf(listing,"%s: void var\n", e->name);
        }
        if (e->defn->kind.dec == ArrayK){
            if (e->defn->type == Integer)
                fprintf(listing,"%s: int array, size %d\n", e->name,e->defn->child[1]->attr.val);
            else if (e->defn->type == Void)
                fprintf(listing,"%s: void array, size %d\n", e->name,e->defn->child[1]->attr.val);
        }
        if (e->defn->kind.dec == FunK){
            if (e->defn->type == Integer){
                fprintf(listing,"%s: int func (", e->name);
                TreeNode * param = e->defn->child[1];
                while (param != NULL){
                    if (param->type == Integer)
                        fprintf(listing,"int");
                    if (param->type == Void)
                        fprintf(listing,"void");
                    if (param->sibling != NULL)
                        fprintf(listing,", ");
                    param = param->sibling;
                }
                fprintf(listing,")\n");
            }
            else if (e->defn->type == Void)
                fprintf(listing,"%s: void func\n", e->name);
        }
    }
}

int hash (char * c){
    int h = 0;
    int i = 0;
    while (c[i] != '\0'){
        h = ((h << SHIFT) + c[i]) % MAXSIZE;
        i++;
    }
    return h;
}

Entry * lookup(char * name,int level){
    Entry * temp;
    int index = hash(name);
    temp = table[index];
    if (temp == NULL){
        return NULL;
    }
    if (strcmp(name,temp->name) == 0 && temp->depth == level){
        return temp;
    }
    return NULL;
}

void printTable(Entry ** table,int level){
    int i;  
    for (i = 0; i < MAXSIZE; i++){
        if (table[i] != NULL){
            Entry * temp = table[i];
            while (temp != NULL && temp->depth == level){
                printEntry(temp);
                temp = temp->next;
            }
        }
    }
}

void delDepth(int level){
    int i = 0;
    for (i = 0; i < MAXSIZE; i++){
        if (table[i] != NULL){
            Entry * temp = table[i];
            while (temp != NULL && temp->depth == level){
                temp = temp->next;
            }
            table[i] = temp;
        }
    }
}

void initTable(Entry ** table){
    int i;
    for (i = 0; i < MAXSIZE; i++){
        table[i] = NULL;
    }
}

Entry * newEntry(char * c, int depth, TreeNode * def){
    Entry * e = malloc(sizeof(Entry));
    e->name = malloc(sizeof(char)*strlen(c));
    strcpy(e->name,c);
    e->defn = def;
    e->depth = depth;
    e->next = NULL;
    return e;
}




int insert (char * c, int depth, TreeNode * def){
    int key = hash(c);
    Entry * temp;
    if (table[key] != NULL){
        temp = table[key];
        table[key] = newEntry(c,depth,def);
        table[key]->next = temp;
    }
    else{
        table[key] = newEntry(c,depth,def);
    }
}

// int main(void){
//     initTable(table);
//     insert("x",0,NULL);
//     insert("x",0,NULL);
//     insert("x",1,NULL);
//     printTable(table);
// }