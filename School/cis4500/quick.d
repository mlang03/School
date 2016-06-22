import std.stdio;
import std.algorithm:swap;
import std.datetime;
import std.random;

int partition(int array[], int low, int high){
    int pivot = array[low];
    int left = low;
    int i;
    for (i = low + 1; i < high; i++){
        if (array[i] < pivot){
            left++;
            swap(array[i],array[left]);
        }
    }
    swap(array[low],array[left]);
    return left;
}

void quickSort(int array[], int low, int high){
    if ( low < high ){
        int pivot = partition(array,low,high);
        quickSort(array,low,pivot);
        quickSort(array,pivot+1,high);
    }
}


void main(){
    int a[];
    int j,i;
    readf("%d",&j);
    a.length = j;
    for (i = 0; i < j; i++){
        a[i] = uniform(1,10000000);
    }
    StopWatch sw;
    sw.start();
    quickSort(a,0,j);
    sw.stop();
    writeln(cast(double)sw.peek().msecs/1000);
}