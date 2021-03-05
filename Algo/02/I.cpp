/*

Дана последовательность целых чисел длины n. Нужно найти количество различных непустых подпоследовательностей.

Входные данные
На первой строке n. На второй строке n целых чисел.

Выходные данные
Количество различных подпоследовательностей по модулю 109+7.

Примеры
входные данные
3
1 1 2
выходные данные
5
входные данные
35
1 2 3 4 5 6 7 8 9 10 1 2 3 4 5 6 7 8 9 10 1 2 3 4 5 6 7 8 9 10 1 2 3 4 5
выходные данные
941167856

*/


#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>


int main() {
    typedef unsigned long long val_t;

    const val_t MOD = 1000000007;

    unsigned n = 0;
    int res = scanf("%u", &n);
    assert(res == 1);

    std::vector<val_t> seq(n, 0);

    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%llu", &seq[i]);
        assert(res == 1);
    }

    std::vector<val_t> dp(n + 1, 0);
    std::vector<val_t> last(n + 1, (val_t)-1);

    dp[0] = 1;

    for (unsigned i = 0; i < n; ++i) {
        dp[i + 1] = (dp[i] * 2) % MOD;

        if (last[seq[i]] != (val_t)-1) {
            dp[i + 1] = (dp[i + 1] - dp[last[seq[i]]] + MOD) % MOD;
        }

        last[seq[i]] = i;
    }

    printf("%llu\n", dp[n] - 1);

    /*for (unsigned i = 0; i <= n; ++i) {
        printf("%u ", dp[i]);
    } printf("\n");*/

    return 0;
}


/*

Dp holds the number of distinct subsequences on a prefix,
Last holds the index of the last occurrence of a number (+1 to have 0 as a flag of no occurrence)

~Also, I initially used arrays, but received constant WA9's, so screw them~
Apparently the problem was the way I took the modulo - the correct way (((n) % mod + mod) % mod) works incorrectly on cf...

*/

