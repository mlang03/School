/* vcftool.c

Unix filter style program for reading/writing and performing operations on vcards.
Made for cis2750 assignment 2.

Created: feb 10th 2014
Author: Michael Lang 0733368
Contact: mlang03@uoguelph.ca

*/

#define _XOPEN_SOURCE 700
#include "vcutil.h"
#include "vcftool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pcre.h>
#include <ctype.h>

int main(int argc, char**argv){
    if (argc == 1){
        fprintf(stderr,"You must enter atleast 1 command.\n");
        return EXIT_FAILURE;
    }
    /*Read in file from STDIN*/
    int write = 0;
    VcStatus writeStatus, readStatus;
    VcFile input;
    readStatus = readVcFile(stdin,&input);
    if (readStatus.code != OK){
        fprintf(stderr,"Read Vcfile failed with error code %d lines %d to %d.\n",readStatus.code,readStatus.linefrom,readStatus.lineto);
        return EXIT_FAILURE;
    }
    /*Run the sepcified command*/
    if (argc > 1){
        if (strcmpML("-info",argv[1]) == 0){
            if (argc > 2) return EXIT_FAILURE;
            vcfInfo(stdout,&input);
        }
        else if (strcmpML("-sort",argv[1]) == 0){
            if (argc > 2) return EXIT_FAILURE;
            vcfSort(&input);
            writeStatus = writeVcFile(stdout,&input);
            write = 1;
        }
        else if (strcmpML("-canon",argv[1]) == 0){
            if (argc > 2) return EXIT_FAILURE;
            vcfCanon(&input);
            writeStatus = writeVcFile(stdout,&input);
            write = 1;
        }
        else if (strcmpML("-select",argv[1]) == 0){
            if (argc < 3){
                fprintf(stderr,"ERROR: You must enter which cards to select.\n");
            }
            else{
                char * which = strdup(argv[2]);
                assert(which);
                
                int check = scanWhich(which);
                if (check == -1){
                    fprintf(stderr,"ERROR: Invalid format for selection.");
                }
                else{
                    vcfSelect(&input,which);
                    writeStatus = writeVcFile(stdout,&input);
                    write = 1;
                }
                free(which);
            }
        }
        else{
            fprintf(stderr,"ERROR: Invalid options.\n");
            freeVcFile(&input);
            return EXIT_FAILURE;
        }
    }
    /*Print the error*/
    if ((write == 1) && writeStatus.code == IOERR){
        fprintf(stderr,"IOERR on linefrom: %d. to lineto: %d.\n",writeStatus.linefrom,writeStatus.lineto);
    }
    /*Free and return*/
    freeVcFile(&input);
    return EXIT_SUCCESS;
}

int scanWhich(char * which){
    int p,u,g,i,length;
    length = strlen(which);
    p = u = g = 0;
    for (i = 0; i < length; i++){
        if (which[i] == 'p') p++;
        else if (which[i] == 'u') u++;
        else if (which[i] == 'g') g++;
        else return -1;
    }
    if (p > 1 || u > 1 || g > 1)
        return -1;
    return 0;
}

int vcfCanProp( VcProp *const propp ){
    
    /*note: REFERENCE: the regex code in this section has been 
     * LARGLY copied/influenced by 
     * http://www.ncbi.nlm.nih.gov/IEB/ToolBox/C_DOC/lxr/source/regexp/demo/pcredemo.c*/
    
    //Declaring regex stuff
    pcre *re;
    const char *error;
    int erroffset;
    int ovector[30];
    int rc;
    
    if (propp->name == VCP_GEO){
        char * namecpy = strdup(propp->value);
        assert(namecpy);
        re = pcre_compile(
            "(-?90\\.0{6}|-?[0-8]?[0-9]\\.[0-9]{6});(-?180\\.0{6}|-?1?[0-7]?[0-9]\\.[0-9]{6})",0,&error,&erroffset,NULL);
        
        if (re == NULL){
            fprintf(stderr,"Regex failed to compile\n");
            return 3;
        }
        /*Attempting to match with the canon form of geo*/
        rc = pcre_exec(re,NULL,namecpy,(int)strlen(namecpy),0,0,ovector,30);
        
        if (rc < 0){
            switch(rc)
            {
                case PCRE_ERROR_NOMATCH:
                    pcre_free(re);
                    re = pcre_compile(
                    "(-?90\\.0{6}|-?[0-8]?[0-9]\\.[0-9]{0,6})[;, ](-?180\\.0{6}|-?1?[0-7]?[0-9]\\.[0-9]{0,6})",0,&error,&erroffset,NULL);
                    /*Now attempting to put geo in canon form*/
                     rc = pcre_exec(re,NULL,namecpy,(int)strlen(namecpy),0,0,ovector,30);
                    
                    if (rc < 0){
                        free(namecpy);
                        pcre_free(re);
                        return 3;
                    }
                    else{
                        /*Replacing space or , with a semi colon*/
                        int substring_length = ovector[1] - ovector[0];
                        if (substring_length == strlen(namecpy)){
                            char * temp;
                            if ((temp = strchr(namecpy,',')) != NULL){
                                temp[0] = ';';
                                free(propp->value);
                                propp->value = strdup(namecpy);
                            }
                            else{
                                temp = strchr(namecpy,' ');
                                temp[0] = ';';
                                free(propp->value);
                                propp->value = strdup(namecpy);
                            }
                            pcre_free(re);
                            free(namecpy);
                            return 2;
                        }
                        else{
                            pcre_free(re);
                            free(namecpy);
                            return 3;
                        }
                    }
                    break;
                default:free(namecpy); pcre_free(re); return 3; break;
            }
        }
        else{
            /*There is a match!!*/
            int substring_length = ovector[1] - ovector[0];
            if (substring_length == strlen(namecpy)){
                free(namecpy);
                pcre_free(re);
                return 1;
            }
            else{
                free(namecpy);
                pcre_free(re);
                return 3;
            }
        }
    }   
    else if (propp->name == VCP_N){
        char * namecpy = strdup(propp->value);
        assert(namecpy);
        re = pcre_compile(
        "(\\(none\\)|[A-Z][a-z ']*);(\\(none\\)|[A-Z][a-z ']*)",0,&error,&erroffset,NULL);
                          
        if (re == NULL){
            fprintf(stderr,"Regex failed to compile\n");
            free(namecpy);
            return 3;
        }
        /*Attempting to match the regex for canon name*/
        rc = pcre_exec(re,NULL,namecpy,(int)strlen(namecpy),0,0,ovector,30);
        
        if (rc < 0){
            switch(rc)
            {
                case PCRE_ERROR_NOMATCH:
                pcre_free(re);
                re = pcre_compile(
                "[a-z][a-z0-9 ']*;[a-z][a-z0-9 ']*",0,&error,&erroffset,NULL);
                /*Attempting to put name into canon form*/
                rc = pcre_exec(re,NULL,namecpy,(int)strlen(namecpy),0,0,ovector,30);
                    
                if (rc < 0){
                    if (strchr(namecpy,';') == NULL){
                        if (strcmp(namecpy,"\0") == 0){
                            free(propp->value);
                            propp->value = malloc(sizeof(char)*20);
                            strcpy(propp->value,"(none);(none)\0");
                            free(namecpy);
                            pcre_free(re);
                            return 2;
                        }
                        propp->value = realloc(propp->value, sizeof(char)*(strlen(propp->value)+10));
                        assert(propp->value);
                        propp->value[0] = toupper(propp->value[0]);
                        strcat(propp->value,";(none)\0");
                        free(namecpy);
                        pcre_free(re);
                        return 2;
                    }
                    if (namecpy[0] == ';'){
                        free(propp->value);
                        propp->value = malloc(sizeof(char)*strlen(namecpy)+20);
                        assert(propp->value);
                        strcpy(propp->value,"(none)\0");
                        char * temp = strchr(namecpy+1,';');
                        if (temp != NULL){
                            if (temp[1] == '\0'){
                                strcat(propp->value,";(none)\0");
                            }
                            else if (temp[1] == ';'){
                                strcat(propp->value,";(none);\0");
                            }
                            else{
                                temp[1] = toupper(temp[1]);
                            }
                        }
                        else{
                            strcat(propp->value,";(none)\0");
                        }
                        free(namecpy);
                        pcre_free(re);
                        return 2;
                    }
                    else{
                        propp->value[0] = toupper(propp->value[0]);
                        propp->value = realloc(propp->value, (strlen(propp->value)+20)*sizeof(char));
                        char * temp = strchr(namecpy+1,';');
                        if (temp != NULL){
                            if (temp[1] == '\0'){
                                strcat(propp->value,"(none)\0");
                            }
                            else if (temp[1] == ';'){
                                strcat(propp->value,"(none);\0");
                            }
                            else{
                                temp[1] = toupper(temp[1]);
                            }
                        }
                    }
                    free(namecpy);
                    pcre_free(re);
                    return 2;
                }
                else{
                    namecpy[0] = toupper(namecpy[0]);
                    char * next = strchr(namecpy,';') +1;
                    next[0] = toupper(next[0]);
                    free(propp->value);
                    
                    propp->value =strdup(namecpy);
                    assert(propp->value);
                    free(namecpy);
                    pcre_free(re);
                    return 2;
                }
                break;
                default:free(namecpy); pcre_free(re); return 3; break;
            }
        }
        else{
            free(namecpy);
            pcre_free(re);
            return 1;
        }
    }
    else if (propp->name == VCP_TEL){
        return 1;
    }
    else if (propp->name == VCP_ADR){
        return 1;
    }
        
    else return 0;
}

int vcfCanon( VcFile *const filep ){
    int i,j;
    for (i = 0; i < filep->ncards; i++){
        Vcard * tempCard = filep->cardp[i];
        int N,A,T,G,rtn;
        N = A = T = G = -1;
        int hasUID = 0;
        char * oldUID;
        VcProp * oldUIDp;
        for (j = 0; j < tempCard->nprops; j++){
            VcProp * tempProp = &tempCard->prop[j];
            /*Going through the cards properties to find info*/
            if (tempProp->name == VCP_GEO){
                rtn = vcfCanProp(tempProp);
                if ((rtn == 1 || rtn == 2) && G != 0){
                    G = 1;
                }
                else if (rtn == 3){
                    G = 0;
                }
            }
            else if (tempProp->name == VCP_N){
                rtn = vcfCanProp(tempProp);
                if ((rtn == 1 || rtn == 2) && N != 0){
                    N = 1;
                }
                else if (rtn == 3){
                    N = 0;
                }
            }
            else if (tempProp->name == VCP_TEL){
                rtn = vcfCanProp(tempProp);
                if ((rtn == 1 || rtn == 2) && T != 0){
                    T = 1;
                }
                else if (rtn == 3){
                    T = 0;
                }
            }
            else if (tempProp->name == VCP_ADR){
                rtn = vcfCanProp(tempProp);
                if ((rtn == 1 || rtn == 2) && A != 0){
                    A = 1;
                }
                else if (rtn == 3){
                    A = 0;
                }
            }
            else if (tempProp->name == VCP_UID){
                oldUIDp = tempProp;
                hasUID = 1;
                oldUID = strdup(tempProp->value);
                assert(oldUID);
            }
        }
        /*Appending the UID to the card*/
        if (hasUID == 0){
            int num;
            num = filep->cardp[i]->nprops + 1;
            filep->cardp[i] = (Vcard*) realloc(filep->cardp[i],sizeof(Vcard) + sizeof(VcProp)*(num));
            filep->cardp[i]->nprops++;
            tempCard = filep->cardp[i];
            
            
            tempCard->prop[num-1].name = VCP_UID;
            tempCard->prop[num-1].parval = NULL;
            tempCard->prop[num-1].partype = NULL;
            tempCard->prop[num-1].hook = NULL;
            tempCard->prop[num-1].value = NULL;
            tempCard->prop[num-1].value = calloc(12,sizeof(char));
            
            /*Writing UID status*/
            strcpy(tempCard->prop[num-1].value,"@----@\0");
            if (N == 1) tempCard->prop[num-1].value[1] = 'N';
            else if (N == 0) tempCard->prop[num-1].value[1] = '*';
            else if (N == -1) tempCard->prop[num-1].value[1] = '-';
            if (G == 1) tempCard->prop[num-1].value[4] = 'G';
            else if (G == 0) tempCard->prop[num-1].value[4] = '*';
            else if (G == -1) tempCard->prop[num-1].value[4] = '-';
            if (A == 1) tempCard->prop[num-1].value[2] = 'A';
            else if (A == 0) tempCard->prop[num-1].value[2] = '*';
            else if (A == -1) tempCard->prop[num-1].value[2] = '-';
            if (T == 1) tempCard->prop[num-1].value[3] = 'T';
            else if (T == 0) tempCard->prop[num-1].value[3] = '*';
            else if (T == -1) tempCard->prop[num-1].value[3] = '-';
        }
        else{
            free(oldUIDp->value);
            oldUIDp->value = NULL;
            oldUIDp->value = calloc(strlen(oldUID)+12,sizeof(char));
            
            /*Writing UID status*/
            strcpy(oldUIDp->value,"@----@\0");
            strcat(oldUIDp->value,";\0");
            strcat(oldUIDp->value,oldUID);
            if (N == 1) oldUIDp->value[1] = 'N';
            else if (N == 0) oldUIDp->value[1] = '*';
            else if (N == -1) oldUIDp->value[1] = '-';
            if (G == 1) oldUIDp->value[4] = 'G';
            else if (G == 0) oldUIDp->value[4] = '*';
            else if (G == -1) oldUIDp->value[4] = '-';
            if (A == 1) oldUIDp->value[2] = 'A';
            else if (A == 0) oldUIDp->value[2] = '*';
            else if (A == -1) oldUIDp->value[2] = '-';
            if (T == 1) oldUIDp->value[3] = 'T';
            else if (T == 0) oldUIDp->value[3] = '*';
            else if (T == -1) oldUIDp->value[3] = '-';
            free(oldUID);
        }
    }
    return EXIT_SUCCESS;
}
   
void getWhich(char * which, int *p, int *u, int *g){
    int i,length;
    length = strlen(which);
    for (i = 0; i < length; i++){
        if (which[i] == 'p') *p = *p + 1;
        else if (which[i] == 'u') *u = *u + 1;
        else if (which[i] == 'g') *g = *g + 1;
    }
}

int vcfSort( VcFile *const filep ){
    Vcard ** array = filep->cardp;
    size_t num = (size_t) filep->ncards;
    size_t size = sizeof(Vcard*);
    
    qsort(array,num,size,compareCards);
    return EXIT_SUCCESS;
}

int isolateSelection(Vcard *** cardp,int ncards, VcPname selection){
    /*Creating a new card array to replace the old one*/
    Vcard ** new = malloc(sizeof(Vcard*)*ncards);
    assert(new);
    int length = ncards;
    int total = 0;
    int i,j;
    for (i = 0; i < length; i++){
        Vcard * tempCard =  (*cardp)[i];
        
        for (j = 0; j < tempCard->nprops; j++){
            VcProp * tempProp = &tempCard->prop[j];
            
            if (tempProp->name == selection){
                /*Copy the card to the new array*/
                total++;
                copyVcard(&new[total-1],(*cardp)[i]);
                break;
            }
        }
    }
    for (i = 0; i < ncards; i++){
        freeVcard((*cardp)[i]);
    }
    free(*cardp);
    *cardp = new;
    return total;
}

int vcfSelect( VcFile *const filep, const char *which ){
    int p,u,g;
    p = u = g = 0;
    char * which2 = strdup(which);
    assert(which2);
    getWhich(which2,&p,&u,&g);
    free(which2);
    int remaining = 0;
    if (p == 1){
        remaining = isolateSelection(&(filep->cardp),filep->ncards,VCP_PHOTO);
        filep->ncards = remaining;
    }
    if (u == 1){
        remaining = isolateSelection(&(filep->cardp),filep->ncards,VCP_URL);
        filep->ncards = remaining;
    }
    if (g == 1){
        remaining = isolateSelection(&(filep->cardp),filep->ncards,VCP_GEO);
        filep->ncards = remaining;
    }
    
    if (remaining == 0){
        fprintf(stderr,"No cards selected.\n");
    }
    return EXIT_SUCCESS;
}

int compareCards(const void * c1, const void * c2){
    Vcard* card1;
    Vcard* card2;
    card1 = *(Vcard**) c1;
    card2 = *(Vcard**) c2;
    char * n1;
    char * n2;
    int i;
    for (i = 0; i < card1->nprops; i++){
        VcProp * check = &(card1->prop[i]);
        if (check->name == VCP_N){
            n1 = strdup(check->value);
            assert(n1);
            break;
        }
    }
    for (i = 0; i < card2->nprops; i++){
        VcProp * check = &(card2->prop[i]);
        if (check->name == VCP_N){
            n2 = strdup(check->value);
            assert(n2);
            break;
        }
    }
    char *n1first = NULL;
    char *n2first = NULL;
    char *n1last = NULL;
    char *n2last = NULL;
    char *temp1 = NULL;
    char *temp2 = NULL;
    
    /*Isolating first and last names into their own strings*/
   
    temp1 = strchr(n1,';');
    if (temp1 != NULL){
        temp1++;
        int size; 
        size = (int) (temp1 - n1) - 1; 
        n1last = calloc(size+1,sizeof(char));
        assert(n1last);
        strncpy(n1last,n1,size);
        temp2 = strchr(temp1,';');
        if (temp2 != NULL){
            temp2++;
            int size2; 
            size2 = (int) (temp2 - temp1) - 1; 
            n1first = calloc(size2+1,sizeof(char));
            assert(n1first);
            strncpy(n1first,temp1,size2);
        }
        else{
            int len = strlen(temp1);
            if (len == 0){
                n1first = calloc(1,sizeof(char));
                assert(n1first);
            }
            else{
                n1first = calloc(len+1,sizeof(char));
                assert(n1first);
                strcpy(n1first,temp1);
            }
        }
    }
    else{
        n1last = strdup(n1);
        assert(n1last);
        n1first = calloc(1,sizeof(char));
        assert(n1first);
    }
    
    temp1 = strchr(n2,';');
    if (temp1 != NULL){
        temp1++;
        int size; 
        size = (int) (temp1 - n2) - 1; 
        n2last = calloc(size+1,sizeof(char));
        assert(n2last);
        strncpy(n2last,n2,size);
        temp2 = strchr(temp1,';');
        if (temp2 != NULL){
            temp2++;
            int size2; 
            size2 = (int) (temp2 - temp1) - 1; 
            n2first = calloc(size2+1,sizeof(char));
            assert(n2first);
            strncpy(n2first,temp1,size2);
        }
        else{
            int len = strlen(temp1);
            if (len == 0){
                n2first = calloc(1,sizeof(char));
                assert(n2first);
            }
            else{
                n2first = calloc(len+1,sizeof(char));
                assert(n2first);
                strcpy(n2first,temp1);
            }
        }
    }
    else{
        n2last = strdup(n1);
        assert(n2last);
        n2first = calloc(1,sizeof(char));
        assert(n2first);
    }
    int rtn;
    
    if (strcmp(n1first,"(none)\0") == 0){
        free(n1first);
        n1first = calloc(1,sizeof(char));
    }
    if (strcmp(n1last,"(none)\0") == 0){
        free(n1last);
        n1last = calloc(1,sizeof(char));
    }
    if (strcmp(n2first,"(none)\0") == 0){
        free(n2first);
        n2first = calloc(1,sizeof(char));
    }
    if (strcmp(n2last,"(none)\0") == 0){
        free(n2last);
        n2last = calloc(1,sizeof(char));
    }
    
    if ((rtn = strcmpML(n1last,n2last)) == 0){
        rtn = strcmpML(n1first,n2first);
    }
    
    if (n1 != NULL) free(n1);
    if (n1first != NULL) free(n1first);
    if (n1last != NULL) free(n1last);
    if (n2 != NULL) free(n2);
    if (n2first != NULL) free(n2first);
    if (n2last != NULL) free(n2last);
    
    return rtn;
}

int vcfInfo( FILE *const outfile, const VcFile *filep ){
    int numCards = 0;
    int sorted = 1;
    int numPhoto = 0;
    int numUrl = 0;
    int numGeo = 0;
    int numCanon = 0;
    
    int i,j,k;
    
    for (i = 0; i < filep->ncards; i++){
        
        Vcard * tempCard =  (filep->cardp[i]);
        int p,u,g;
        p = u = g = 0;
        /*Checking the card's props for info*/
        for (j = 0; j < tempCard->nprops; j++){
            VcProp * tempProp = &tempCard->prop[j];
            if (tempProp->name == VCP_PHOTO && !p){
                numPhoto++;
                p = 1;
            }
            if (tempProp->name == VCP_URL && (u == 0)){
                numUrl = numUrl + 1;
                u = 1;
            }
            if (tempProp->name == VCP_GEO && !g){
                numGeo++;
                g = 1;
            }
            if (tempProp->name == VCP_UID){
                char * uid = strdup (tempProp->value);
                assert(uid);
                if (uid[0] == '@' || uid[5] == '@'){
                    int add = 1;
                    for (k = 1; k < 5; k++){
                        if (uid[k] == '*')
                            add = 0;
                    }
                    if (add == 1)
                        numCanon++;
                }
                free(uid);
            }
        }
        numCards++;
    }
    for (i = 0; i < filep->ncards-1; i++){
        int result = compareCards(&(filep->cardp[i]),&(filep->cardp[i+1]));
        if (result > 0){
            sorted = 0;
            break;
        }
    }
    /*print out the info*/
    if (sorted == 0)
        fprintf(outfile,"%d cards (not sorted)\n",numCards);
    else
        fprintf(outfile,"%d cards (sorted)\n",numCards);
    fprintf(outfile,"%d with photos\n",numPhoto);
    fprintf(outfile,"%d with URLs\n",numUrl);
    fprintf(outfile,"%d with geographical coordinates\n",numGeo);
    fprintf(outfile,"%d in canonical form\n",numCanon);
    return (EXIT_SUCCESS);
}
