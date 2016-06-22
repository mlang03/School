#!/usr/bin/python3.4


#BY MICHAEL LANG
#3150 A4
#Nov 11 2014

def peek(stack):
    return stack[-1]

def notEmpty(stack):
    if not stack:
        return 0
    else:
        return 1

def showStacks():
    print(stackA)
    print(stackB)
    print(stackC)
    
def checkifsorted():
    if len(stackA) < N:
        return False
    for i in range(N-1,1,-1):
        if not stackA[i] or stackA[i] != i:
            return False
    return True
        
    
def moveCard():
    
    #RULES
    
    #All 3 stacks non-empty
    if (notEmpty(stackA) + notEmpty(stackB) + notEmpty(stackC)) == 3:
        
        if peek(stackA) > peek(stackB):
            #Move A to C
            value = stackA.pop()
            stackC.append(value)
        else:
            #Move B to C
            value = stackB.pop()
            stackC.append(value)
        
    #2 stacks non-empty
    if (notEmpty(stackA) + notEmpty(stackB) + notEmpty(stackC)) == 2:
        
        #Stack A empty
        if notEmpty(stackA) == 0:
            #Move B to C
            value = stackB.pop()
            stackC.append(value)
            
        if notEmpty(stackB) == 0:
            #Move C to A
            value = stackC.pop()
            stackA.append(value)
        
        if notEmpty(stackC) == 0:
            #Move B to A
            value = stackB.pop()
            stackA.append(value)
            
    #Only 1 stack non-empty
    if (notEmpty(stackA) + notEmpty(stackB) + notEmpty(stackC)) == 1:
        
        if notEmpty(stackA):
            #Move A to B
            value = stackA.pop()
            stackB.append(value)
            
        elif notEmpty(stackC):
            #Move C to A
            value = stackC.pop()
            stackA.append(value)
        else:
            #Move A to C
            value = stackA.pop()
            stackC.append(value)
            

stacksSizeInput = input("Enter stack sizes n1 n2 n3: ")
stackSizes = [int(x) for x in stacksSizeInput.split() if x.isdigit()]

N = sum(stackSizes)

stackAInput = input("Enter stack 1: ")
stackBInput = input("Enter stack 2: ")
stackCInput = input("Enter stack 3: ")

stackA = [int(x) for x in stackAInput.split()]
stackB = [int(x) for x in stackBInput.split()]
stackC = [int(x) for x in stackCInput.split()]

while(not checkifsorted()):
    moveCard()
    showStacks()
    
#for i in range(10):
#    moveCard()
#    showStacks()


#Shows the result
showStacks()