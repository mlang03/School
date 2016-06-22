/********
vcftool.h -- header file for vcftool.c in Asmt 2
Last updated:  January 28, 2014 10:05:00 PM       

By: Michael Lang 0733368
Created: feb 10th 2014
Contact: mlang03@uoguelph.ca
********/
#ifndef VCFTOOL_H_
#define VCFTOOL_H_ A2

#include "vcutil.h"

int vcfInfo( FILE *const outfile, const VcFile *filep );
int vcfCanon( VcFile *const filep );
int vcfSelect( VcFile *const filep, const char *which );
int vcfSort( VcFile *const filep );

int vcfCanProp( VcProp *const propp );


/*Compare function used for Qsort*/
int compareCards(const void * c1,const void * c2);

/*Runs an error test to see if the string is a valid choice for select*/
int scanWhich(char * which);

/*Parses the selection string for use by vcfselect*/
void getWhich(char * which, int *p, int *u, int *g);

/*Isolates the array of cards so that the remaining list only contains cards with the selection*/
int isolateSelection(Vcard *** cardp,int ncards, VcPname selection);

#endif
