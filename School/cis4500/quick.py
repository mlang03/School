import random
import time
def partition(array, low, high):
    pivot = array[low]
    left = low
    for i in range(low+1,high):
        if array[i] < pivot:
            left = left + 1
            array[i], array[left] = array[left], array[i]
    array[low], array[left] = array[left], array[low]
    return left

def quickSort(array,low,high):
    if (low < high):
        pivot = partition(array,low,high)
        quickSort(array,low,pivot)
        quickSort(array,pivot+1,high)
 
A = []

size = int(raw_input(""))
for i in range(0,size):
    A.append(random.randrange(0,10000000))
t = time.time()
quickSort(A,0,len(A))
print((time.time()-t))