/********
vcutil.h -- header file for vcutil.c
Last updated:  Feb 9th, 2014 4:20pm  

By:Michael Lang 0733368
Created: feb 10th 2014
Contact: mlang03@uoguelph.ca
********/
#ifndef VCUTIL_H_
#define VCUTIL_H_ A2
#include <stdio.h>

#define FOLD_LEN 75     // fold lines longer than this length (RFC 2426 2.6)
#define VCARD_VER "3.0" // version of standard accepted

#define CR 13
#define LF 10
#define SPACE 32
#define TAB 9

/* SYmbols for recognized vCard property names */

typedef enum {
    VCP_BEGIN,
    VCP_END,
    VCP_VERSION,
    VCP_N,      // name
    VCP_FN,     // formatted name
    VCP_NICKNAME,
    VCP_PHOTO,
    VCP_BDAY,
    VCP_ADR,    // address
    VCP_LABEL,  // address formatted for printing
    VCP_TEL,
    VCP_EMAIL,
    VCP_GEO,    // lat,long
    VCP_TITLE,
    VCP_ORG,
    VCP_NOTE,
    VCP_UID,    // unique ID
    VCP_URL,
    VCP_OTHER   // used for any other property name
} VcPname;


/* data structures for vCard file in memory */

typedef struct {    // property (=contentline)
    VcPname name;       // property name

    // storage for 0-2 parameters (NULL if not present)
    char *partype;      // TYPE=string
    char *parval;       // VALUE=string
    
    char *value;        // property value string
    void *hook;         // reserved for pointer to parsed data structure
} VcProp;

typedef struct {    // single card
    int nprops;         // no. of properties
    VcProp prop[];      // array of properties
} Vcard;

typedef struct {    // vCard file
    int ncards;         // no. of cards in file
    Vcard **cardp;      // pointer to array of card pointers
} VcFile;


/* General status return from functions */

typedef enum { OK=0,
    IOERR,      // I/O error
    SYNTAX,     // property not in form 'name:value'
    PAROVER,    // parameter overflow
    BEGEND,     // BEGIN...END not found as expected
    BADVER,     // version not accepted
    NOPVER,     // no "VERSION" property
    NOPNFN,     // no "N" or "FN" property
} VcError;
    
typedef struct {
    VcError code;               // error code
    int linefrom,lineto;        // line numbers where error occurred
} VcStatus;   


/* File I/O functions */

VcStatus readVcFile( FILE *const vcf, VcFile *const filep );
VcStatus readVcard( FILE *const vcf, Vcard **const cardp );
VcStatus getUnfolded( FILE *const vcf, char **const buff );
VcError parseVcProp( const char *buff, VcProp *const propp );
VcStatus writeVcFile( FILE* const vcf, const VcFile *filep );
void freeVcFile( VcFile *const filep );

/*my function for comparing case insensitive strings
 * return: 0 if false 1 if true
 * params: the strings to be compared*/
int strcmpML(char * string1, char * string2);

void copyVcard(Vcard ** dest, Vcard * source);
void freeVcard(Vcard * card);

#endif
