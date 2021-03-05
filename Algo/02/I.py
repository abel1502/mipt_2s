def f(n, seq):
    assert n == len(seq)
    assert n >= max(seq)
    
    MOD = 10 ** 9 + 7
    
    dp = [0 for _ in range(n + 1)]
    last = [-1 for _ in range(n + 1)]
    
    dp[0] = 1
    
    for i in range(n):
        dp[i + 1] = (dp[i] * 2) % MOD
        
        if last[seq[i]] != -1: 
            dp[i + 1] -= dp[last[seq[i]]]
            dp[i + 1] %= MOD
        
        last[seq[i]] = i
    
    return dp[n] - 1


n = int(input())
a = list(map(int, input().split()))

print(f(n, a))