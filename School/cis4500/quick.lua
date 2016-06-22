
function partition(array, low, high)
    pivot = array[low]
    left = low
    for i = low + 1, high do
        if array[i] < pivot then
            left = left + 1
            array[i], array[left] = array[left], array[i]
        end
    end
    array[low], array[left] = array[left], array[low]
    return left
end

function quickSort(array, low, high)
    if (low < high) then
        pivot = partition(array, low, high)
        quickSort(array, low, pivot)
        quickSort(array, pivot + 1, high)
    end
end

A = {}
asize = io.read("*number")
for i = 1,asize do 
    A[i] = math.random(10000000)
end
stime = os.clock()
quickSort(A,1,#A)
print(os.clock()-stime)
