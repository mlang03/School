/* vcutil.c

Utility functions for reading and parsing vcards.
Made for cis2750 assignment 1/2.

Created: Jan 24 2014 updated feb 10th 2013
Author: Michael Lang 0733368
Contact: mlang03@uoguelph.ca

*/

#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vcutil.h"
#include <assert.h>

#define CR 13
#define LF 10
#define SPACE 32
#define TAB 9


/*returns the number of paramters in the string
 * return: The number of parameters, 
 * params: the strings to be checked*/
int getNumParams(char * string);

/*returns the integer representation of the VcPname
 * return: the integer representation of the vcpname
 * params: the string to be checked*/
int getVcPname(char * name);

/*checks for syntax errors in the string
 * return: 0 if no errors else 1
 * params: the string to be checked*/
int checksyntax(char * string);

/*Checks for optional groups
 * return: 0 if false 1 if true
 * params: the string to be checked*/
int checkforgroup(char * string);

/*checks if the string has a space in it
 * return: 0 if false 1 if true
 * params: the string to be checked*/
int checkSpace(char * string);

/*Free's any allocated data inside a property
 * return: void
 * params: the prop to be freed*/
void freeProp(VcProp * prop);

/*Copys the data from 1 prop to another 
 * return: void
 * params: properties to be copied*/
void copyProp(VcProp *dest, VcProp *source);

/*copys 1 vcard into another 
 * return: void
 * params: the cards to be copied*/
void copyVcard(Vcard ** dest, Vcard * source);

/*frees the content of 1 vcard
 * return: void
 * params: the card to be freed*/
void freeVcard(Vcard * card);

char * getVcPnameString(int num);

VcStatus writeVcProp(FILE *const vcf, VcProp * prop);

VcStatus writeVcard(FILE *const vcf, Vcard * card); 

int foldLine(char ** line);


VcStatus getUnfolded( FILE *const vcf, char **const buff ){
    static int totalLines = 0;
    static char lastchar;
    static int lineNum;
    int lineto = 0;
    int lineExtend = 0;
    int blankLinesAfter = 0;
    int blankLinesPrior = 0;
    
    char * line;
    line = malloc(sizeof(char)*400);
    char * temp = line;
    char * temp2;
    
    //if its not the first line, insert the 
    if (totalLines != 0){
        line[0] = lastchar;
        temp++;
    }

    char read; 
    
    //Special case: reset all statics and return
    if (vcf == NULL){
        totalLines = 0;
        lastchar = 0;
        lineNum = 0;
        VcStatus status;
        status.code = 0;
        status.linefrom = 0;
        status.lineto = 0;
        return status;
    }
    /*Booleans*/
    int reading = 1;
    int isLine = 0;
    int onlyspace = 1;
    int spaceZ = 400;
    int sizeCount = 0;
    
    /*################################
     * Unfolding the line.
     * ##############################*/
    while (reading){
        
    /*################################
     * Read up until the first CR.
     * ##############################*/

        while ((read = fgetc(vcf)) != CR && (read != LF)){

            if (read != EOF){
                if (read != SPACE && (read !=TAB)) onlyspace = 0; //onlyspace ensures that we dont read any only whitespace lines
                if (onlyspace == 0){
                    *temp++ = read;
                    sizeCount++;
                }
                if (onlyspace == 0) isLine = 1; //reading atleast one character from the line means that the line is valid.
                if (sizeCount >= spaceZ-1){
                    spaceZ = spaceZ * 2;
                    line = realloc(line,sizeof(char)*spaceZ);
                    temp = line + sizeCount;
                }
            }
            else if (read == EOF){
                reading = 0; //end the line at EOF
                break;
            }
        }
        
        
        if (isLine == 1){
            fgetc(vcf); //Eat LF
            if ((read = fgetc(vcf)) == SPACE || (read == TAB)){ //Continue reading the next line.
                lineExtend++; //increment the linecounter
            }
            else{
                if (read == CR) blankLinesAfter++; //BLANK LINE AHEAD
                if (read == CR || read == LF)
                    while ((read = fgetc(vcf)) == CR || (read == LF) || (read == SPACE) || (read == TAB)){ //FIND THE NEXT READ AHEAD CHAR
                        if (read == '\r') blankLinesAfter ++;
                    }
                    if (read == EOF && blankLinesAfter != 0){
                            blankLinesAfter++;
                    }
                lastchar = read;
                reading = 0;
            }
        }
        else{ //NOT VALID LINE... LEADING BLANK LINES AHEAD
            if (read == CR || read == LF){
                blankLinesPrior++;
                while ((read = fgetc(vcf)) == CR || (read == LF)|| (read == SPACE) || (read == TAB)){
                        if (read == CR) blankLinesPrior ++;
                }
                *temp++ = read;
                sizeCount++;
            }
        }
    }
     *temp = '\0';
    
    /*################################
     * Copy the string into the buffer.
     * ##############################*/
    if (isLine){
        *buff = (char *) malloc(sizeof(char)*strlen(line)+1);
        temp = *buff;
        temp2 = line;
        while (*temp2){
            *temp++ = *temp2++;
        }
        *temp = '\0';
    }
    else{
        *buff = NULL;
    }
    free(line);
    
    /*################################
     * Update number of lines read
     * ##############################*/
    if (isLine) totalLines += blankLinesPrior;
    if (isLine) totalLines++;
    lineNum = totalLines;
    lineto = totalLines;
    if (isLine){
        if (lineExtend != 0){
            lineto = lineExtend + totalLines;
            totalLines+=lineExtend;
        }
    }
    /*################################
     * Preparing VcStatus to return;
     * ##############################*/
    VcStatus status;
    if (isLine){
        status.linefrom = lineNum;
        status.lineto = lineto;
    }
    else{
       status.linefrom = totalLines;
       status.lineto = totalLines;
    }
    totalLines += blankLinesAfter;
    status.code = OK;
    return status;
}


VcError parseVcProp( const char *buff, VcProp *const propp ){
    char * tempType;
    char * temp;
    
    //initialize all fields to null
    propp->name = -1;
    propp->partype = NULL;
    propp->parval = NULL;
    propp->value = NULL;
    propp->hook = NULL;
    
    temp = strdup(buff);
    
    //check the buffer for basic syntax errors
    int syntax = checksyntax(temp);
    if (syntax == 1){
        if (temp!= NULL) free(temp);
        return SYNTAX;
    }
    
    //copy the property name and paramters
    int size; 
    size = (int) (strchr(temp,':') - temp); 
    tempType = calloc(size+1,sizeof(char));
    strncpy(tempType,temp,size);
    
    //copy the value
    propp->value = calloc(strlen(temp)-size+1,sizeof(char));
    strcpy(propp->value,strchr(temp,':')+1);
    
    //get the number of paramters and do another basic syntax check
    int numParams = getNumParams(tempType);
    if (numParams == -1){
        freeProp(propp);
        free(tempType);
        free(temp);
        return SYNTAX;
    }
    char * getName = tempType;
    
    //remove any groups from the type
    if (checkforgroup(tempType) == 1){
        getName = strchr(tempType,'.') + 1;
    }
    
    //get the property name
    if (numParams == 0){
        propp->name = getVcPname(getName);
    }
    else{
        size = (int) (strchr(tempType,';') - tempType);
        getName = calloc(size+1,sizeof(char));
        strncpy(getName,tempType,size);
        propp->name = getVcPname(getName);
        free(getName);
    }
    
    if (propp->name == VCP_OTHER){
        if (propp->value != NULL) free(propp->value);
        propp->value = strdup(temp);
        free(temp);
        free(tempType);
        return OK;
    }
    
    
    //parse the parameters.
    int numTypes = 0;
    int numValues = 0;
    int i;
    
    char * currentParam; 
    char * params;
    
    if (numParams > 0){
        params = strchr(tempType,';') + 1;
    }
    currentParam = params;
    for (i = 0; i < numParams; i++){
        char delim = ';';
        int lastParam = 0;
        if (i == numParams -1) lastParam = 1;
        
        //get the parameter type
        size = (int) (strchr(currentParam,'=') - currentParam);
        char * paramtype = calloc(size+1,sizeof(char));
        strncpy(paramtype,currentParam,size);
        
        currentParam = strchr(currentParam,'=')+ 1;
        
        //get the parameter value
        char * value;
        (lastParam == 1) ? size = strlen(currentParam) : (size = (int) (strchr(currentParam,delim) - currentParam));
        value = calloc(size+1,sizeof(char));
        strncpy(value,currentParam,size);
        
        //if param type is type or value, store them
        if (strcmpML(paramtype,"type") == 0){
            if (numTypes == 0){
                propp->partype = strdup(value);
                numTypes ++;
            }
            else{
                propp->partype = realloc(propp->partype, (sizeof(char)* (strlen(propp->partype) +strlen(value)) +2));
                strcat(propp->partype,",\0");
                strcat(propp->partype,value);
            }
        }
        else if (strcmpML(paramtype,"value") == 0){
            if (numValues == 0){
                propp->parval = strdup(value);
                numValues ++;
            }
            else{
                propp->parval = realloc(propp->parval, (sizeof(char)* (strlen(propp->parval) +strlen(value)) +2));
                strcat(propp->parval,",\0");
                strcat(propp->parval,value);
            }
        }
        //if parameter type is charset or encoding ignore them
        else if (strcmpML(paramtype,"charset") != 0 && strcmpML(paramtype,"encoding") != 0){
            //if parameter type empty return syntax error.
            if (strcmpML(paramtype,"") == 0){
                free(tempType);
                free(temp);
                freeProp(propp);
                free(paramtype);
                free(value);
                return SYNTAX;
            }
            //if parameter type has a space in it return syntax error.
            if (checkSpace(paramtype) == 1){
                free(tempType);
                free(temp);
                freeProp(propp);
                free(paramtype);
                free(value);
                return SYNTAX;
            }
            //else return parover
            freeProp(propp);
            free(tempType);
            free(temp);
            free(paramtype);
            free(value);
            return PAROVER;
        }
        currentParam = strchr(currentParam,delim) + 1;
        free(paramtype);
        free(value);
    }
    free(tempType);
    free(temp);
    return OK;
}

VcStatus readVcard( FILE *const vcf, Vcard **const cardp ){
    VcStatus tempStat;
    VcStatus rtn;
    char * line;
    int numProps = 2;
    int curLineTo;
    int curLineFrom;
    VcProp first,second; //used for holding begin and version properties.
    *cardp = NULL;
    
    /*EAT AND CHECK THE FIRST 2 PROPERTIES*/
    tempStat =getUnfolded(vcf,&line);
    if (line != NULL){
        parseVcProp(line,&first);
        if (first.name != VCP_BEGIN || strcmpML(first.value,"VCARD") != 0){
            rtn.code = BEGEND;
            rtn.linefrom = tempStat.linefrom;
            rtn.lineto = tempStat.lineto;
            return rtn;
        }
        free(line);
        freeProp(&first);
        
        tempStat = getUnfolded(vcf,&line);
        if (line != NULL) 
        {
            parseVcProp(line,&second);
            if (second.name != VCP_VERSION){
                rtn.code = NOPVER;
                rtn.linefrom = tempStat.linefrom;
                rtn.lineto = tempStat.lineto;
                return rtn;
            }
            else {
                if (strcmpML(second.value,"3.0") != 0){
                    rtn.code = BADVER;
                    rtn.linefrom = tempStat.linefrom;
                    rtn.lineto = tempStat.lineto;
                    return rtn;
                }
            }
        }
        else{
                rtn.code = BEGEND;
                rtn.linefrom = tempStat.linefrom;
                rtn.lineto = tempStat.lineto;
                return rtn;
        }
        free(line);
        freeProp(&second);
    }
    else{
        rtn.code = OK;
        rtn.linefrom = tempStat.linefrom;
        rtn.lineto = tempStat.lineto;
        return rtn;
    }
    
    
    //read in the rest of the properties
    VcProp temp;
    VcError error;
    int count = 0;
    
    //flags for checking required properties.
    int hasN = 0;
    int hasFN = 0;
    int end = 0;
    int i;
    
    do{
        tempStat = getUnfolded(vcf,&line);
        curLineTo = tempStat.lineto;
        curLineFrom = tempStat.linefrom;
        if (line != NULL){
            numProps++;
            //parse the property
            error = parseVcProp(line,&temp);
            //check for errors
            if (error != OK){
                freeProp(&temp);
                for (i = 0; i < count; i++){
                    freeProp( &((*cardp)->prop[i]) );
                }
                rtn.code = error;
                rtn.linefrom = curLineFrom;
                rtn.lineto = curLineTo;
                free(*cardp);
                free(line);
                return rtn;
            }
            //Store the property.
            if (temp.name != VCP_END){
                if (temp.name == VCP_N) hasN = 1;
                if (temp.name == VCP_FN) hasFN = 1;
                if (temp.name == VCP_BEGIN){
                    freeProp(&temp);
                    for (i = 0; i < count; i++){
                        freeProp( &((*cardp)->prop[i]) );
                    }
                    free(*cardp);
                    free(line);
                    rtn.code = BEGEND;
                    rtn.linefrom = curLineFrom;
                    rtn.lineto = curLineTo;
                    return rtn;
                }
                count++;
                *cardp = realloc(*cardp,sizeof(Vcard) + (sizeof(VcProp)*count));
                (*cardp)->prop[count-1].partype = NULL;
                (*cardp)->prop[count-1].parval = NULL;
                (*cardp)->prop[count-1].value = NULL;
                (*cardp)->nprops = count;
                copyProp(&((*cardp)->prop[count-1]),&temp);
                freeProp(&temp);
            }
            //End the vcard.
            else{
                if (strcmpML(temp.value,"VCARD") == 0) end = 1;
                freeProp(&temp);
                free(line);
                break;
            }
            free(line);
        }
    } while (line != NULL);
    
    //If missing one of the essential properties, free and exit.
    if (end != 1 || hasN != 1 || hasFN != 1){
        for (i = 0; i < count; i++){
            freeProp( &((*cardp)->prop[i]) );
        }
        if (end == 0) rtn.code = BEGEND;
        else rtn.code = NOPNFN;
        rtn.linefrom = curLineFrom;
        rtn.lineto = curLineTo;
        return rtn;
    }
    //return the valid vcard.
    rtn.code = OK;
    rtn.linefrom = curLineFrom;
    rtn.lineto = curLineTo;
    return rtn;
}

VcStatus readVcFile( FILE *const vcf, VcFile *const filep ){
    VcStatus rtn;
    VcStatus status;
    int numCards = 1;
    filep->cardp = malloc(sizeof(Vcard*)*1);
    do{
        //reallocate storage based on the number of parameters
        filep->cardp = realloc(filep->cardp, sizeof(Vcard*)*numCards);
        filep->ncards = numCards;
        status = readVcard(vcf,&filep->cardp[numCards-1]);
        if (status.code == OK ){
            if ((filep->cardp[numCards-1]) == NULL){
                filep->ncards -= 1;
                break;
            }
            else{
                numCards++;
            }
        }
        //return the error to the calling program.
        else{
            rtn.code = status.code;
            rtn.linefrom = status.linefrom;
            rtn.lineto = status.lineto;
            freeVcard(*filep->cardp);
            return rtn;
        }
    }
    while (status.code == OK);
    rtn.code = status.code;
    rtn.linefrom = status.linefrom;
    rtn.lineto = status.lineto;
    return rtn;
}

VcStatus writeVcFile( FILE *const vcf, const VcFile *filep ){
    int i;
    VcStatus rtn;
    rtn.code = OK;
    rtn.lineto = 1;
    rtn.linefrom = 1;
    if (vcf == NULL) return rtn;
    int lineCount = 0;
    VcStatus check;
    for (i = 0; i < filep->ncards; i++){
        check = writeVcard(vcf, filep->cardp[i]);
        lineCount += check.lineto;
        if (check.code == IOERR){
            rtn.code = IOERR;
            rtn.linefrom = check.linefrom;
            rtn.lineto = check.linefrom;
            return rtn;
        }
    }
    rtn.code = OK;
    rtn.linefrom = lineCount;
    rtn.lineto = lineCount;
    return rtn;
}

VcStatus writeVcard(FILE *const vcf, Vcard * card){
    int i;
    VcStatus rtn;
    if (fputs("BEGIN:VCARD",vcf) < 0){
        rtn.code = IOERR;
        rtn.linefrom = 1;
        rtn.lineto = 1;
        return rtn;
    }
    fputc(CR,vcf);
    fputc(LF,vcf);
    
    fputs("VERSION:",vcf);
    fputs(VCARD_VER,vcf);
    fputc(CR,vcf);
    fputc(LF,vcf);
    
    int lineCount = 2;
    VcStatus check;
    for (i = 0; i < card->nprops; i++){
        check = writeVcProp(vcf, &card->prop[i]);
        if (check.code == OK){
            lineCount += check.lineto;
        }
        else if (check.code == IOERR){
            rtn.code = IOERR;
            rtn.linefrom = check.linefrom;
            rtn.lineto = check.lineto;
            return rtn;
        }
    }
    
    fputs("END:VCARD",vcf);
    fputc(CR,vcf);
    fputc(LF,vcf);
    lineCount++;
    
    rtn.code = OK;
    rtn.lineto = lineCount;
    rtn.linefrom = lineCount;
    return rtn;
}

VcStatus writeVcProp(FILE *const vcf, VcProp * prop){
    char * temp = calloc(400,sizeof(char));
    assert(temp);
    
    char endLine[3] = {CR,LF,0};
    
    VcStatus rtn;
    rtn.code = OK;
    rtn.lineto = 1;
    rtn.linefrom = 1;
    
    char * line = temp;
    char * tempName;
    int length = 0;
    int size = 1;
    if (prop->name == VCP_OTHER){
        strcpy(temp,prop->value);
        length = strlen(temp);
        if (length > 75){
            size = foldLine(&temp);
            rtn.lineto = size;
        }
        else{
            strcat(line,endLine);
        }
        fputs(temp,vcf);
        free(temp);
        return rtn;
    }
    
    tempName = getVcPnameString(prop->name);
    assert(tempName);
    
    strcpy(line,tempName);
    free(tempName);
    
    if (prop->partype != NULL){
        if (strlen(prop->partype) >= 100){
            temp = realloc(temp,sizeof(char)*(strlen(temp) + strlen(prop->partype) + 2));
            line = temp;//line = temp + strlen(prop->partype);
        }
        strcat(line,";TYPE=\0");
        strcat(line,prop->partype);
    }
    
    if (prop->parval != NULL){
        if (strlen(prop->parval) >= 100){
            temp = realloc(temp,sizeof(char)*(strlen(temp) + strlen(prop->parval) + 2));
            line = temp;
        }
        strcat(line,";VALUE=\0");
        strcat(line,prop->parval);
    }
    
    if (strlen(prop->value) >= 100){
        temp = realloc(temp,sizeof(char)*(strlen(temp) + strlen(prop->value) + 2));
        line = temp;
    }
    strcat(line,":\0");
    strcat(line,prop->value);
    
    length = strlen(temp);
    if (length > 75){
        size = foldLine(&temp);
    }
    else{
        strcat(line,endLine);
    }
    
    if (fputs(temp,vcf) < 0){
        free(temp);
        rtn.code = IOERR;
        rtn.linefrom = 1;
        rtn.lineto = size;
        return rtn;
    }
    
    free(temp);
    rtn.code = OK;
    rtn.linefrom = 1;
    rtn.lineto = size;
    return rtn;
}

int foldLine(char ** line){
    
    char * old = *line;
    
    char fold[4] = {CR,LF,SPACE,0};
    char endLine[3] = {CR,LF,0};
    
    int numFolds = strlen(*line)/75 + 1;
    int count = numFolds;
    
    int space = numFolds*4 + strlen(*line) + 5;
    
    char * temp = calloc(space,sizeof(char));
    assert(temp);
    
    int first = 1;
    while(count > 0){
        if (first){
            strncat(temp,*line,75);
            *line += 75;
            first = 0;
        }
        else{
            strncat(temp,*line,74);
            *line += 74;
        }
        if (count != 1){
            strcat(temp,fold);
        }
        else{
            strcat(temp,endLine);
        }
        count--;
    }
    *line = realloc(old,sizeof(char)*space);
    assert(*line);
    strcpy(*line,temp);
    free(temp);
    return numFolds;
}

int strcmpML(char * string1, char * string2){
    int i; 
    char *str1 = strdup(string1);
    char *str2 = strdup(string2);
    for (i = 0; i< strlen(str1); i++){
        if (str1[i] > 96 && str1[i] < 123){
            str1[i] = str1[i] - 32;
        }
    }
    for (i = 0; i< strlen(str2); i++){
        if (str2[i] > 96 && str2[i] < 123){
            str2[i] = str2[i] - 32;
        }
    }
    int value = strcmp(str1,str2);
    free(str1);
    free(str2);
    return (value);
}

void freeProp(VcProp * prop){
    if (prop == NULL) {
        return;
    }
    
    if (prop->partype != NULL){
        free(prop->partype);
        prop->partype = NULL;
    }
    
    if (prop->parval != NULL){
        free(prop->parval);
        prop->parval = NULL;
    }
    
    if (prop->value != NULL){
        free(prop->value);
        prop->value = NULL;
    }
}

void freeVcard(Vcard * card){
    if (card == NULL) return;
    int i;
    for (i = 0; i < card->nprops; i++){
        if (&card->prop[i] != NULL) freeProp(&card->prop[i]);
    }
    free(card);
}

void freeVcFile( VcFile *const filep ){
    if (filep == NULL) return;
    int i;
    for (i = 0; i < filep->ncards; i++){
        freeVcard(filep->cardp[i]);
    }
    free(filep->cardp);
}

void copyProp(VcProp *dest, VcProp *source){
    dest->name = source->name;
    dest->partype = NULL;
    dest->parval = NULL;
    dest->value = NULL;
    dest->hook = NULL;
    if (source->partype != NULL) dest->partype = strdup(source->partype);
    if (source->parval != NULL) dest->parval = strdup(source->parval);
    if (source->value != NULL) dest->value = strdup(source->value);
}

void copyVcard(Vcard ** dest, Vcard * source){
    int i;
    if (source != NULL){
//         *dest = realloc(*dest,sizeof(Vcard) + sizeof(VcProp)*source->nprops);
        *dest = malloc(sizeof(Vcard) + sizeof(VcProp)*source->nprops);
        (*dest)->nprops = source->nprops;
        for (i = 0; i < (*dest)->nprops; i++){
            copyProp(&((*dest)->prop[i]),&source->prop[i]);
        }
    }
}

int getNumParams(char * string){
    int numsemi = 0, numequal = 0, i;
    for (i = 0;i < strlen(string);i++){
        if (string[i] == ';') numsemi++;
    }
    for (i = 0;i < strlen(string);i++){
        if (string[i] == '=') numequal++;
    }
    if (numequal == numsemi)
        return numsemi;
    else return -1;
}

int checkSpace(char * string){
    int i;
     for (i = 0;i < strlen(string); i++){
        if (string[i] == SPACE) return 1;
    }
    return 0;
}


int checksyntax(char * string){
    int i, numcolon = 0, numSemiColon = 0;
    if (string[0] == ':'|| string[0] == ';') return 1;
    for (i = 0;i < strlen(string); i++){
        if (string[i] == ':') numcolon++;
        if (string[i] == ';') numSemiColon++;
        if (string[i] == ' ' && (numcolon == 0 && numSemiColon == 0)) return 1;
    }
    if (numcolon == 0) return 1;
    else return 0;
}

int checkforgroup(char * string){
    int i;
    for (i = 0;i < strlen(string);i++){
        if (string[i] == ';' || string[i] == ':'){
            break;
        }
        if (string[i] == '.') return 1;
    }
    return 0;
}

int getVcPname(char * name){
    if (strcmpML(name,"BEGIN") == 0) return 0;
    else if (strcmpML(name,"END") == 0) return 1;
    else if (strcmpML(name,"VERSION") == 0) return 2;
    else if (strcmpML(name,"N") == 0) return 3;
    else if (strcmpML(name,"FN") == 0) return 4;
    else if (strcmpML(name,"NICKNAME") == 0) return 5;
    else if (strcmpML(name,"PHOTO") == 0) return 6;
    else if (strcmpML(name,"BDAY") == 0) return 7;
    else if (strcmpML(name,"ADR") == 0) return 8;
    else if (strcmpML(name,"LABEL") == 0) return 9;
    else if (strcmpML(name,"TEL") == 0) return 10;
    else if (strcmpML(name,"EMAIL") == 0) return 11;
    else if (strcmpML(name,"GEO") == 0) return 12;
    else if (strcmpML(name,"TITLE") == 0) return 13;
    else if (strcmpML(name,"ORG") == 0) return 14;
    else if (strcmpML(name,"NOTE") == 0) return 15;
    else if (strcmpML(name,"UID") == 0) return 16;
    else if (strcmpML(name,"URL") == 0) return 17;
    else return 18;
}

char * getVcPnameString(int num){
    char * name =  malloc(sizeof(char)*10);
    if (num == 0) strcpy(name,"BEGIN\0");
    else if (num == 1) strcpy(name,"END\0");
    else if (num == 2) strcpy(name,"VERSION\0");
    else if (num == 3) strcpy(name,"N\0");
    else if (num == 4) strcpy(name,"FN\0");
    else if (num == 5) strcpy(name,"NICKNAME\0");
    else if (num == 6) strcpy(name,"PHOTO\0");
    else if (num == 7) strcpy(name,"BDAY\0");
    else if (num == 8) strcpy(name,"ADR\0");
    else if (num == 9) strcpy(name,"LABEL\0");
    else if (num == 10) strcpy(name,"TEL\0");
    else if (num == 11) strcpy(name,"EMAIL\0");
    else if (num == 12) strcpy(name,"GEO\0");
    else if (num == 13) strcpy(name,"TITLE\0");
    else if (num == 14) strcpy(name,"ORG\0");
    else if (num == 15) strcpy(name,"NOTE\0");
    else if (num == 16) strcpy(name,"UID\0");
    else if (num == 17) strcpy(name,"URL\0");
    else strcpy(name,"OTHER\0");
    return name;
}