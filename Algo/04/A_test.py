import subprocess
import io
import random


def test(cmds, ans):
    inData = "{cmds}\n".format(n=len(cmds), cmds='\n'.join(cmds))
    result = subprocess.run("A.exe", input=inData.encode(), stdout=subprocess.PIPE)
    return result.returncode == 0 and int(result.stdout.decode().split()[0]) == ans


def genTest(minN=3, maxN=10, maxH=15):
    n = random.randrange(minN, maxN)
    b = random.randrange(1, 9)
    a = random.randrange(b + 1, 10)
    
    h = [random.randrange(1, maxH) for _ in range(n)]
    
    bestHits = float("inf")
    bestSeq = None
    
    maxH = max([x // a + 1 for x in h])
    maxH = max(maxH, h[0] // b + 1, h[n - 1] // b + 1) + 1
    
    for i in range(maxH ** n):
        seq = [0] + [(i // maxH ** j) % maxH for j in range(1, n - 1)] + [0]
        
        h2 = list(h)
        
        for i in range(1, n - 1):
            h2[i] -= a * seq[i]
            h2[i - 1] -= b * seq[i]
            h2[i + 1] -= b * seq[i]
        
        if all(map(lambda x: x < 0, h2)) and bestHits > sum(seq):
            bestHits = sum(seq)
            bestSeq = seq
    
    cmds = list(map(str, [n, a, b] + h))
        
    return bestHits, cmds, bestSeq


def stress(count):
    for i in range(count):
        ans, cmds, bestSeq = genTest(maxN=10, maxH=10)
        
        if not test(cmds, ans):
            print("[Error]", cmds, ans, bestSeq, flush=True)
            #break
        
        print('.')


stress(100)

"""

7 9 1
6 2 3 2 7 2 3


13
[0, 7, 0, 1, 1, 4, 0]

6 2 3 2 7 2 3
- - - - - 4
6 2 3 2 3 # #
- - - - 1
6 2 3 1 # # #
- - - 


===

6 2 3
- 7
# # #




[Error] ['9', '8', '1', '4', '9', '8', '8', '5', '5', '4', '2', '2'] 13 [0, 5, 1, 1, 1, 2, 0, 3, 0]


"""