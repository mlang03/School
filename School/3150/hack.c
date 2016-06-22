#include <stdio.h>

int main(void){

    int n = 0;
    while(1){
        n = -~n;
        printf("%d\n",n);
    }
    return 0;
}