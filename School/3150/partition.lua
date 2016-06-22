--CIS 3150 A5
--By: Michael Lang
--Oct 22nd 2014
--PARTITION PROBLEM

--ANSWERS
--S = { 4, 5, 20, 21, 30, 40, 86, 96 }
        --i)S is in PARTITION 
        --ii)S has 2 partitions with the same sum

--S = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }
        --i)S is not in PARTITION

--S = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 }
        --i)S is in PARTITION
        --ii)S has 27 partitions with the same sum

function genBinaryLex( t, set)
    if t > N then 
        process( binaryString, set )
    else
        binaryString[t] = '0'
        genBinaryLex( t + 1 ,set )
        binaryString[t] = '1'
        genBinaryLex( t + 1 ,set )
    end
end

function process( binary, set )
    bitstring = ""
    for i,bit in ipairs( binary ) do
        bitstring = bitstring .. bit
    end

    index = 0
    sumOnes = 0
    sumZeroes = 0
    for bit in bitstring:gmatch("%d") do
        index = index + 1
        if tonumber(bit) == 1 then
            sumOnes = sumOnes + set[index]
        end
        if tonumber(bit) == 0 then
            sumZeroes = sumZeroes + set[index]
        end
    end
    
    if sumOnes == sumZeroes then
        if set == set1 then
            set1Partitions = set1Partitions + 1
        end
        if set == set2 then
            set2Partitions = set2Partitions + 1
        end
        if set == set3 then
            set3Partitions = set3Partitions + 1
        end
    end
    
end


set1 = { 4, 5, 20, 21, 30, 40, 86, 96 }
set2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }
set3 = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 }

set1Partitions = 0
set2Partitions = 0
set3Partitions = 0

binaryString = {}

N = #set1
genBinaryLex( 1, set1 )

N = #set2
genBinaryLex( 1, set2 )

N = #set3
genBinaryLex( 1, set3 )

print("Number of partitions in Set 1: " .. set1Partitions/2)
print("Number of partitions in Set 2: " .. set2Partitions/2)
print("Number of partitions in Set 3: " .. set3Partitions/2)