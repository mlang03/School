/* Header defining types and constants and includes other headers
 * Author: Michael Lang
 * Feb 4th 2016
*/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define token_max_length 40

typedef enum {
    END_FILE,
    OPEN_TAG,
    CLOSE_TAG,
    WORD,
    NUMBER,
    APOSTROPHIZED,
    HYPHENATED,
    PUNCTUATION,
    ERROR
} Token_type;

typedef enum {
    TEXT,
    DATE,
    DOC,
    DOCNO,
    HEADLINE,
    LENGTH,
    P
} Tag_type;

Token_type getToken(void);

#define max_stack_size 1000



#endif