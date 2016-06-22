/* Stack data structure for strings
 * Author: Michael Lang
 * Feb 4th 2016
*/

#include "stack.h"
#include "globals.h"

static int size = 0;
extern char stack[max_stack_size][token_max_length+1];

void push(char * s){
    strcpy(stack[size], s);
    size++;
}

char * pop(void){
    if (size == 0){
        return NULL;
    }
    char * temp = malloc(sizeof(char)*strlen(stack[size-1])+1);
    strcpy(temp, stack[size-1]);
    stack[size-1][0] = '\0';
    size--;
    return temp;
}
char * peek(void){
    if (size == 0){
        return NULL;
    }
    return stack[size-1];
}

void printStack(void){
    int i;
    for (i = 0; i < size; i++){
        printf("%s\n",stack[i]);
    }
}

int getSize(void){
    return size;
}
