import subprocess
import io
import random


def test(cmds):
    inData = "{cmds}\n".format(n=len(cmds), cmds='\n'.join(cmds))
    result = subprocess.run("C.exe", input=inData.encode(), stdout=subprocess.PIPE)
    result2 = subprocess.run("C_reference.exe", input=inData.encode(), stdout=subprocess.PIPE)
    return result.returncode == result2.returncode and result.stdout.decode() == result2.stdout.decode()


def genTest(minK=1, maxK=10, maxA=10 ** 18):
    k = random.randrange(minK, maxK)
    
    cmds = [random.randrange(1, maxA) for _ in range(k)]
    
    cmds = [str(k)] + list(map(str, cmds))
        
    return cmds


def stress(count):
    for i in range(count):
        cmds = genTest(maxK=20)
        
        if not test(cmds):
            print("[Error]", cmds)
            #break
        
        #if i % 20 == 0:
        #    print('.', end='')


stress(100)

"""
[Error] ['2', '318678120018747175', '599942346376573642']
2
318678120018747175 599942346376573642

287138037/861414112

correct:
953804709/861414112
"""