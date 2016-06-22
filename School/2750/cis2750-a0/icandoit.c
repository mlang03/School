/* icandoit.c
 * Assignement 0 for 2750. Jan 8th 2014
 * This file contains 2 functions. One that returns my student number, and the other
 * returns the checksum for the provided integer.
 * By: Michael Lang 0733368
 */ 

int getStuNum(){
    return 733368;
}

unsigned int checksum(unsigned int num){
    if ( num < 10) return num;
    else return (num % 10 + checksum(num/10));
}
