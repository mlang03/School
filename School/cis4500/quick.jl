function partition(array, low, high)
    pivot = array[low]
    left = low
    for i = low+1:high
        if array[i] < pivot
            left = left + 1
            array[i], array[left] = array[left], array[i]
        end
    end
    array[low], array[left] = array[left], array[low]
    return left;
end

function quickSort(array, low, high)
    if low < high
        pivot = partition(array,low,high)
        quickSort(array,low,pivot)
        quickSort(array,pivot+1,high)
    end
end

s = int(readline(STDIN))
x = cell(s)
for i = 1:s
    x[i] = rand(1:10000000)
end
@time quickSort(x,1,s)