import subprocess
import io
import random


def test(cmds, ans):
    inData = "{cmds}\n".format(n=len(cmds), cmds='\n'.join(cmds))
    result = subprocess.run("I.exe", input=inData.encode(), stdout=subprocess.PIPE)
    return result.returncode == 0 and int(result.stdout.decode()) == ans


def genTest(minN=1, maxN=10):
    n = random.randrange(minN, maxN)
    
    cmds = [random.randrange(0, n) for _ in range(n)]
    subseqs = {()}
    
    for i in cmds:
        for ss in subseqs.copy():
            subseqs.add(ss + (i,))
    
    ans = (len(subseqs) - 1) % (10 ** 9 + 7)
    cmds = [str(n)] + list(map(str, cmds))
        
    return ans, cmds


def stress(count):
    for i in range(count):
        ans, cmds = genTest(maxN=20)
        
        if not test(cmds, ans):
            print("[Error]", cmds, ans)
            #break
        
        if i % 20 == 0:
            print('.', end='')


stress(10000)

"""


"""