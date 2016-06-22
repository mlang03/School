--NFA Simulation
--Coded in Lua
--By: Mike Lang
--CIS3150 Assignment #2


--Results
--00111000 = rejected
--e = accepted
--(01)^10 = rejected
--(0^9)(1^7) = accepted

E = 2
EMPTYSET = 3

function main() 
    --Read the bit pattern
    print("Enter the input for the NFA.")
    input_bits = io.read()
    
    --Store them in table
    bit_length = 0
    bit_pattern = {}
    for bit in string.gmatch( input_bits, "%d") do
        table.insert(bit_pattern,tonumber(bit))
        bit_length = bit_length + 1
    end
    
    result = transition(0, bit_pattern, 1, bit_length)
    
    if result == 2 then 
        print("String accepted")
    else
        print("String rejected")
    end
end

function transition( current_state, input, index, maxlen )
    if current_state == EMPTYSET then
        return false
    end
    if index > maxlen then
        --print(current_state)
        if (input[index] == 3) or (input[index] == 5) then 
            return true
        else
            return false
        end
    end
    
    next_state = {}
    if current_state == 0 then 
        if input[index] == 0 then
            table.insert(next_state, 4)
        elseif input[index] == 1 then
            table.insert(next_state, 1)
        end
        table.insert(next_state, 1)
        table.insert(next_state, 3)
        
    elseif current_state == 1 then 
        if input[index] == 0 then
            table.insert(next_state, 1)
        elseif input[index] == 1 then
            table.insert(next_state, 1)
        end
        table.insert(next_state, EMPTYSET)
        
    elseif current_state == 2 then 
        if input[index] == 0 then
            table.insert(next_state, 1)
            table.insert(next_state, 5)
        elseif input[index] == 1 then
            table.insert(next_state, 4)
        end
        table.insert(next_state, EMPTYSET)
        
    elseif current_state == 3 then 
        if input[index] == 0 then
            table.insert(next_state, EMPTYSET)
        elseif input[index] == 1 then
            table.insert(next_state, EMPTYSET)
        end
        table.insert(next_state, EMPTYSET)
        
    elseif current_state == 4 then 
        if input[index] == 0 then
            table.insert(next_state, 4)
        elseif input[index] == 1 then
            table.insert(next_state, 2)
        end
        table.insert(next_state, 0)
        
    elseif current_state == 5 then 
        if input[index] == 0 then
            table.insert(next_state, 1)
        elseif input[index] == 1 then
            table.insert(next_state, 5)
        end
        table.insert(next_state, EMPTYSET)
        
    end
    results = {}
    for i,future_state in ipairs(next_state) do
        result = transition(future_state, input, index + 1, maxlen)
        table.insert(results, result)
    end
    
    for i,result in ipairs(results) do
        if result == true then 
            return true
        end
    end
    return false
end

main()