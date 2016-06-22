--Binary Pattern matching FSA
--Coded in Lua
--By: Mike Lang
--CIS3150 Assignment #1

STATE_START = 1
STATE_MATCH_1 = 2
STATE_MATCH_2 = 3
STATE_MATCH_3 = 4
STATE_MATCH_4 = 5
STATE_MATCH_5_ACCEPT = 6

function main()
    
    --Read the bit pattern
    print("Enter a 5-bit binary string pattern.")
    input_bits = io.read()
    
    --Store them in table
    bit_pattern = {}
    for bit in string.gmatch( input_bits, "%d") do
        table.insert(bit_pattern,tonumber(bit))
    end
    
    --Read the binary Text that we will be processing
    print("Enter the binary text")
    binary_text = io.read()

    --Here we will begin processing the text 1 bit at a time and simulating a FSA
    current_state = STATE_START
    for bit in string.gmatch( binary_text, "%d") do
        current_state = transition(current_state, tonumber(bit), bit_pattern)
    end

    --Now check the final state to see if the string was accepted
    if current_state == STATE_MATCH_5_ACCEPT then
        print("Matching successful.")
    else
        print("Could not match that string.")
    end
end


function transition( state, bit, pattern )
    next_state = state
      
    if state == STATE_START then
        if bit == pattern[1] then
            next_state = STATE_MATCH_1        
        else
            next_state = STATE_START
        end
    
    elseif state == STATE_MATCH_1 then
        if bit == pattern[2] then
            next_state = STATE_MATCH_2
        else
            next_state = STATE_START
        end
    
    elseif state == STATE_MATCH_2 then
        if bit == pattern[3] then
            next_state = STATE_MATCH_3
        else
            next_state = STATE_START
        end
    
    elseif state == STATE_MATCH_3 then
        if bit == pattern[4] then
            next_state = STATE_MATCH_4
        else
            next_state = STATE_START
        end
    
    elseif state == STATE_MATCH_4 then
        if bit == pattern[5] then
            next_state = STATE_MATCH_5_ACCEPT
        else
            next_state = STATE_START
        end
    
    elseif state == STATE_MATCH_5_ACCEPT then
        next_state = state
    end
    
    return next_state
end

main()