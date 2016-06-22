/* Main program for SGML scanner
 * Author: Michael Lang
 * Edited from file given by Fei Song
 * Feb 4th 2016
*/
#include "globals.h"
#include "stack.h"

char * normalize_tag(char * tk, Token_type t);
int checkRelevant(char * tag);
char current[token_max_length+1];

char stack[max_stack_size][token_max_length+1]; //our stack

extern char token[token_max_length+1];

int relevant = 1;   //Boolean: whether or not we are within relevant tags

FILE * input_file;
FILE * output_file;


int main(void){
    input_file = stdin;
    output_file = stdout;
    Token_type toke_type;
    while ((toke_type = getToken()) != END_FILE){
        if (toke_type == OPEN_TAG){
            char * tag = normalize_tag(token,toke_type);
            push(tag);
            strcpy(current,tag);
            free(tag);
            if ((relevant == 1) && !checkRelevant(peek())){ //Check if we've entered an irrelevant tag
                    relevant = 0;
            }
            if (relevant == 1){
                printf("OPEN-%s\n",peek());
            }
        }
        else if (toke_type == CLOSE_TAG){
            char * tag = normalize_tag(token,toke_type);
            char * temp = peek();
            if (strcmp(tag,temp)!= 0){
                fprintf(stderr,"ERROR: Close tag </%s> found, </%s> was expected\n",tag,temp);
            }
            else{
                pop();
                if (relevant == 1){
                    printf("CLOSE-%s\n",tag);
                }
                else{
                    if (checkRelevant(peek())){ //Check if we are back into a relevant tag
                        relevant = 1;
                    }
                }   
            }
        }
        else if (relevant == 0) {
            //eat input
        }
        else if (toke_type == APOSTROPHIZED){
            printf("APOSTROPHIZED(%s)\n",token);
        }
        else if (toke_type == HYPHENATED){
            printf("HYPHENATED(%s)\n",token);
        }
        else if (toke_type == WORD){
            printf("WORD(%s)\n",token);
        }
        else if (toke_type == NUMBER){
            printf("NUMBER(%s)\n",token);
        }
        else if (toke_type == PUNCTUATION){
            printf("PUNCTUATION(%s)\n",token);
        }
    }
    while (getSize() > 0){
        char * temp = pop();
        fprintf(stderr,"ERROR: Unmatched open tag: <%s>\n",temp);
    }
    return 0;
}

/*Function to check if a tag is relevant*/
int checkRelevant(char * tag){
    if ((strcmp(tag,"TEXT") != 0) && 
        (strcmp(tag,"DATE") != 0) && 
        (strcmp(tag,"DOC") != 0) && 
        (strcmp(tag,"DOCNO") != 0) && 
        (strcmp(tag,"HEADLINE") != 0) && 
        (strcmp(tag,"LENGTH") != 0) && 
        (strcmp(tag,"P") != 0)){
        return 0;
    }
    return 1;
}

/*function to convert tag into uppercase and remove <> characters*/
char * normalize_tag(char * tk, Token_type t){
    char * buff = calloc(token_max_length,sizeof(char));
    int i;
    if (t == OPEN_TAG){
        strncpy(buff,tk+1,strlen(tk)-2);
        for (i = 0; i < strlen(buff); i++){
            if (isalpha(buff[i]) && islower(buff[i])){
                buff[i] = toupper(buff[i]);
            }
        }
        return buff;
    }
    else if (t == CLOSE_TAG){
        strncpy(buff,tk+2,strlen(tk)-3);
        for (i = 0; i < strlen(buff); i++){
            if (isalpha(buff[i]) && islower(buff[i])){
                buff[i] = toupper(buff[i]);
            }
        }
        return buff;
    }
}