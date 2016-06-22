#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap (int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int array[], int low, int high){
    int pivot = array[low];
    int left = low;
    int i;
    for (i = low + 1; i < high; i++){
        if (array[i] < pivot){
            left++;
            swap(&(array[i]),&(array[left]));
        }
    }
    swap(&(array[low]),&(array[left]));
    return left;
}

void quickSort(int array[], int low, int high){
    if ( low < high ){
        int pivot = partition(array,low,high);
        quickSort(array,low,pivot);
        quickSort(array,pivot+1,high);
    }
}

int main(void){
    clock_t begin, end;
    double timeElapsed;
    srand(time(NULL));
    int numNums;
    int i;
    scanf("%d",&numNums);
    int * A = malloc(sizeof(int)*numNums);
    for (i = 0; i < numNums; i++){
        A[i] = rand() % 10000000;
    }
    begin = clock();
    quickSort(A,0,numNums+1);
    end = clock();
    timeElapsed = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\n time: %lf\n",timeElapsed);
    return 0;
}