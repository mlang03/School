--CIS 3150 A3
--By: Michael Lang
--Oct 22nd 2014


--ANSWERS:
--A) 22159
--B) 3003
--C) 7

function genBinaryLex( t )
    if t > N then 
        process( binaryString )
    else
        binaryString[t] = '0'
        genBinaryLex( t + 1 )
        binaryString[t] = '1'
        genBinaryLex( t + 1 )
    end
end

function process( binary )
    bitstring = ""
    for i,bit in ipairs( binary ) do
        bitstring = bitstring .. bit
    end
    
    --Check if string has a 000 substring
    if (bitstring:find( "000" ) ~= nil) then
        num000 = num000 + 1
    end
    
    --Check if string has exactly 5 1's and the sum their indexes
    -- if there is exactly 5 1's
    num1s = 0
    index = 0
    sum = 0
    for bit in bitstring:gmatch("%d") do
        index = index + 1
        if tonumber(bit) == 1 then
            num1s = num1s + 1
            sum = sum + index
        end
    end
    if num1s == 5 then
        num5ones = num5ones + 1
        if sum == 20 then
            sum20 = sum20 + 1
        end
    end
end

N = 15
num000 = 0
num5ones = 0
sum20 = 0
binaryString = {}
genBinaryLex(1)
print("Number of binary strings with a 000 substring = " .. num000)
print("Number of binary strings with exactly 5 1's = " .. num5ones)
print("Number of binary strings subsets of size 5 summing to 20 = " .. sum20)