/*

Найдите максимальный вес золота, который можно унести в рюкзаке вместительностью S, если есть N золотых слитков с заданными весами.

Входные данные
В первой строке входного файла запианы два числа — S и N (1\N{LESS-THAN OR SLANTED EQUAL TO}S\N{LESS-THAN OR SLANTED EQUAL TO}10000, 1\N{LESS-THAN OR SLANTED EQUAL TO}N\N{LESS-THAN OR SLANTED EQUAL TO}300). Далее следует N неотрицательных целых чисел, не превосходящих 100\N{THIN SPACE}000 — веса слитков.

Выходные данные
Выведите искомый максимальный вес.

Примеры
входные данные
10 3
1 4 8
выходные данные
9
входные данные
20 4
5 7 12 18
выходные данные
19

*/


#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>
#include <algorithm>


int main() {
    typedef unsigned val_t;

    unsigned s = 0;
    unsigned n = 0;
    int res = scanf("%u %u", &s, &n);
    assert(res == 2);

    std::vector<unsigned> seq(n, 0);

    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%u", &seq[i]);
        assert(res == 1);
    }

    std::vector<std::vector<val_t>> dp(n + 1, std::vector<val_t>(s + 1, 0));
    dp[0][0] = 0;

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned w = 0; w <= s; ++w) {
            dp[i + 1][w] = std::max(dp[i + 1][w], dp[i][w]);

            if (w + seq[i] <= s) {
                dp[i + 1][w + seq[i]] = std::max(dp[i + 1][w + seq[i]], dp[i][w] + seq[i]);
            }
        }

    }

    printf("%u\n", *std::max_element(dp[n].begin(), dp[n].end()));

    return 0;
}


/*

It's the trivial backpack, except the cost is equal to the weight

*/


