#!/usr/bin/python3.4


#SORTER


# paper date: Feb 26th





def push(stack,item):
    stack.append(item)
    
def top(stack):
    if not stack:
        return None
    else:
        return stack[len(stack)-1]
    
def pop(stack):
    if not stack:
        return
    else:
        return stack.pop()

def record(A,B,C):
    return "0".join((''.join(A),''.join(B),''.join(C)))

def moveAtoB(A,B,C):
    push(B,pop(A))

def moveAtoC(A,B,C):
    push(C,pop(A))

def moveBtoA(A,B,C):
    push(A,pop(B))

def moveBtoC(A,B,C):
    push(C,pop(B))

def moveCtoA(A,B,C):
    push(A,pop(C))

def moveCtoB(A,B,C):
    push(B,pop(C))

def move(A,B,C,algorithm):
    topA = top(A)
    topB = top(B)
    topC = top(C)
    
    if (topA) and (topB) and (topC):
        #Variation 1 A > B > C
        if (topA > topB) and (topB > topC) and (topA > topC):
            if (algorithm[0] == '0'):
                moveAtoC(A,B,C)
            elif (algorithm[0] == '1'):
                moveCtoA(A,B,C)
            elif (algorithm[0] == '2'):
                moveBtoA(A,B,C)
            elif (algorithm[0] == '3'):
                moveBtoC(A,B,C)
        #Variation 2 A > C > B
        if (topA > topB) and (topC > topB) and (topA > topC):
            if (algorithm[1] == '0'):
                moveAtoC(A,B,C)
            elif (algorithm[1] == '1'):
                moveCtoA(A,B,C)
            elif (algorithm[1] == '2'):
                moveBtoA(A,B,C)
            elif (algorithm[1] == '3'):
                moveBtoC(A,B,C)
        #Variation 3 B > A > C
        if (topB > topA) and (topA > topC) and (topB > topC):
            if (algorithm[2] == '0'):
                moveAtoC(A,B,C)
            elif (algorithm[2] == '1'):
                moveCtoA(A,B,C)
            elif (algorithm[2] == '2'):
                moveBtoA(A,B,C)
            elif (algorithm[2] == '3'):
                moveBtoC(A,B,C)
        #Variation 4 B > C > A
        if (topB > topA) and (topC > topA) and (topB > topC):
            if (algorithm[3] == '0'):
                moveAtoC(A,B,C)
            elif (algorithm[3] == '1'):
                moveCtoA(A,B,C)
            elif (algorithm[3] == '2'):
                moveBtoA(A,B,C)
            elif (algorithm[3] == '3'):
                moveBtoC(A,B,C)
        #Variation 1 C > A > B
        if (topC > topA) and (topA > topB) and (topC > topB):
            if (algorithm[4] == '0'):
                moveAtoC(A,B,C)
            elif (algorithm[4] == '1'):
                moveCtoA(A,B,C)
            elif (algorithm[4] == '2'):
                moveBtoA(A,B,C)
            elif (algorithm[4] == '3'):
                moveBtoC(A,B,C)
        #Variation 1 C > B > A
        if (topC > topA) and (topB > topA) and (topC > topB):
            if (algorithm[5] == '0'):
                moveAtoC(A,B,C)
            elif (algorithm[5] == '1'):
                moveCtoA(A,B,C)
            elif (algorithm[5] == '2'):
                moveBtoA(A,B,C)
            elif (algorithm[5] == '3'):
                moveBtoC(A,B,C)
    elif topA and topB and not topC:
        if (topA > topB):
            if (algorithm[6] == '0'):
                moveAtoC(A,B,C)
            elif (algorithm[6] == '1'):
                moveBtoA(A,B,C)
            elif (algorithm[6] == '2'):
                moveBtoC(A,B,C)
        else:
            if (algorithm[7] == '0'):
                moveAtoC(A,B,C)
            elif (algorithm[7] == '1'):
                moveBtoA(A,B,C)
            elif (algorithm[7] == '2'):
                moveBtoC(A,B,C)
    elif topA and topC and not topB:
        if (topA > topC):
            if (algorithm[8] == '0'):
                moveAtoB(A,B,C)
            elif (algorithm[8] == '1'):
                moveAtoC(A,B,C)
            elif (algorithm[8] == '2'):
                moveCtoB(A,B,C)
            elif (algorithm[8] == '3'):
                moveCtoA(A,B,C)
        else:
            if (algorithm[9] == '0'):
                moveAtoB(A,B,C)
            elif (algorithm[9] == '1'):
                moveAtoC(A,B,C)
            elif (algorithm[9] == '2'):
                moveCtoB(A,B,C)
            elif (algorithm[9] == '3'):
                moveCtoA(A,B,C)
    elif topB and topC and not topA:
        if (topB > topC):
            if (algorithm[10] == '0'):
                moveCtoA(A,B,C)
            elif (algorithm[10] == '1'):
                moveBtoA(A,B,C)
            elif (algorithm[10] == '2'):
                moveBtoC(A,B,C)
        else:
            if (algorithm[11] == '0'):
                moveCtoA(A,B,C)
            elif (algorithm[11] == '1'):
                moveBtoA(A,B,C)
            elif (algorithm[11] == '2'):
                moveBtoC(A,B,C)
    elif topA and not topB and not topC:
        if algorithm[12] == '0':
            moveAtoB(A,B,C)
        elif algorithm[12] == '1':
            moveAtoC(A,B,C)
    elif topC and not topA and not topB:
        if algorithm[13] == '0':
            moveCtoA(A,B,C)
        elif algorithm[13] == '1':
            moveCtoB(A,B,C)
        

def simulate(algorithm, start_config):
    global num
    #Find the solution string
    n = len(start_config) - 2
    sol = ''.join([str(x) for x in range(n,0,-1)])
    #Split strings into stacks
    split_stack = start_config.split('0')
    stackA = list(split_stack[0])
    stackB = list(split_stack[1])
    stackC = list(split_stack[2])
    
    
    used_configs = []
    
    #print(start_config)
    while(1):
        rec = record(stackA,stackB,stackC)
        if sol in ''.join(stackA):
            #print("Solution!")
            #num = num+1
            #print(rec)
            #print(algorithm)
            #print(start_config)
            return 1
        if rec in used_configs:
            #print("Repeat!")
            #print(rec)
            return 0
        else:
            #print(rec)
            used_configs.append(rec)
        move(stackA,stackB,stackC,algorithm)

    
num = 0
if __name__ == "__main__":
    simulate("21123112100011","001432")
    

    