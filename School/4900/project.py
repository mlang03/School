#!/usr/bin/python3.4

# ------------ 3 stacks non-empty ------------
# NUMBER OF SETUPS: 1
#   Setup 1: STACK A,B,C are non-empty
#   NUMBER OF VARIATIONS: 3! = 6
#       variation 1: A > B > C
#       variation 2: A > C > B
#       variation 3: B > A > C
#       variation 4: B > C > A
#       variation 5: C > A > B
#       variation 6: C > B > A
# 
#   >>Each variation has 4 Different moves
#           1: Move A to C
#           2: Move C to A
#           3: Move B to A
#           4: Move B to C
#
# ------------ 2 stacks non-empty ------------
# NUMBER OF SETUPS = 3
#   Setup 1: STACK A is empty
#   NUMBER OF VARIATIONS: 2! = 2
#       variation 1: B > C
#       variation 1: C > B
#
#   >>3 moves per variation
#       1: Move C to A
#       2: Move B to A
#       3: Move B to C
#
#   Setup 2: STACK B is empty
#   NUMBER OF VARIATIONS: 2! = 2
#       variation 1: A > C
#       variation 1: C > A
#
#   >>4 moves per variation
#       1: Move A to B
#       2: Move A to C
#       3: Move C to B
#       4: Move C to A
#
#   Setup 3: STACK C is empty
#   NUMBER OF VARIATIONS: 2! = 2
#       variation 1: A > B
#       variation 1: B > A
#
#   >>3 moves per variation
#       1:Move A to C
#       2:Move B to A
#       3:Move B to C
#
# ------------ 1 stacks non-empty ------------
#
# NUMBER OF SETUPS = 3
#   Setup 1: STACK A is non-empty
#   NUMBER OF VARIATIONS: 1
#       variation 1: A
#
#   >> 2 moves per variation
#       1: Move A to B
#       2: Move A to C
#
#   Setup 2: STACK B is non-empty
#   NUMBER OF VARIATIONS: 1
#       variation 1: B       >>>>>CANNOT OCCUR, REMOVE FROM ALGORitHM
#
#   >> 2 moves per variation
#
#   Setup 1: STACK A is non-empty
#   NUMBER OF VARIATIONS: 1
#       variation 1: B
#
#   >> 2 moves per variation
#       1: Move C to A
#       2: Move C to B
import random

def runTest(startConfig, algorithm):
    global num
    num = num + 1
    if num % 1000000 == 0:
        print(num)
    #print(startConfig)
    #print(algorithm)
    if random.random() < 0.01:
        return 1
    return 0

# number of configs : N! * (2N + 1)
# number of algorithms : 4^8 * 3^4 * 2^2 = 21,233,664

# number of tests to run N = 3: 891,813,888
# number of tests to run N = 4: 
# number of tests to run N = 5: 


def testExtension(arg1,arg2):
    global num
    num = num + 1
    if num % 1000000 == 0:
        print (num)
        print(">>> " +arg1)
        print(">>> " +arg2)

def genTestConfigs(config, t, n, algorithm):
    global num
    if t > n+1:
        #num = num + 1
        if runTest(config, algorithm) == 0:
            return 0
        else:
            return 1
    else:
        configNext = config[:]
        for i in range(n+1):
            if i == 0:
                if config.count(0) < 2:
                    if config.count(0) == 1:
                        if t - config.index(0) > 2:
                            continue
                    configNext[t] = i
                    if genTestConfigs(configNext, t+1, n, algorithm) == 0:
                        return 0
            elif i not in config:
                configNext[t] = i
                if genTestConfigs(configNext, t+1, n, algorithm) == 0:
                    return 0

def testAlgorithm(algorithm, n):
    configString = [-1 for x in range(n+2)]
    if genTestConfigs(configString, 0, n, algorithm) == 1:
        print("algorithm Accepted")

def genAlgorithms( t, n, algorithm, algolength):
    global num
    if t > algolength + 1:
            #num = num + 1
            testAlgorithm(algorithm, n)
            
    else:
        for i in range(sizes[t]):
            algorithm[t] = i
            genAlgorithms(t+1, n, algorithm, algolength)

num = 0

if __name__ == "__main__":        
    sizes = [4,4,4,4,4,4,3,3,4,4,3,3,2,2]
    string = [0 for x in range(14)]
    #genAlgorithms(0, 3,string,12)
    #configString = [-1 for x in range(6)]
    #genTestConfigs(configString,0,4,sizes)
    print(num)