/*

ƒаны две последовательности. Ќайдите длину их наибольшей общей подпоследовательности (подпоследовательность Ч это то, что можно получить из данной последовательности вычеркиванием некоторых элементов).

¬ходные данные
¬ первой строке входного файла записано число N Ч длина первой последовательности (1\N{LESS-THAN OR SLANTED EQUAL TO}N\N{LESS-THAN OR SLANTED EQUAL TO}1000). ¬о второй строке записаны члены первой последовательности (через пробел) Ч целые числа, не превосход€щие 10\N{THIN SPACE}000 по модулю. ¬ третьей строке записано число M Ч длина второй последовательности (1\N{LESS-THAN OR SLANTED EQUAL TO}M\N{LESS-THAN OR SLANTED EQUAL TO}1000). ¬ четвертой строке записаны члены второй последовательности (через пробел) Ч целые числа, не превосход€щие 10\N{THIN SPACE}000 по модулю.

¬ыходные данные
¬ выходной файл требуетс€ вывести единственное целое число: длину наибольшей общей подпоследовательности, или число 0, если такой не существует.

ѕример
входные данные
3
1 2 3
4
2 1 3 5
выходные данные
2

*/


#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <algorithm>


int main() {
    unsigned n = 0, m = 0;

    int res = scanf("%u", &n);
    assert(res == 1);

    int *seq1 = new int[n];
    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%d", &seq1[i]);
        assert(res == 1);
    }

    res = scanf("%u", &m);
    assert(res == 1);

    int *seq2 = new int[m];
    for (unsigned i = 0; i < m; ++i) {
        res = scanf("%d", &seq2[i]);
        assert(res == 1);
    }

    unsigned dpHeight = n + 1;
    unsigned dpWidth = m + 1;

    // Wow, I actually just found out new[] doesn't zero everything out by default...
    unsigned *dp = new unsigned[dpHeight * dpWidth] {};

    #define DP_(X, Y)    dp[(X) * dpWidth + (Y)]

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j = 0; j < m; ++j) {
            if (seq1[i] == seq2[j]) {
                DP_(i + 1, j + 1) = DP_(i, j) + 1;
            } else {
                DP_(i + 1, j + 1) = std::max(DP_(i, j + 1), DP_(i + 1, j));
            }
        }
    }

    printf("%u\n", DP_(n, m));

    #undef DP_

    delete[] dp;
    delete[] seq1;
    delete[] seq2;

    return 0;
}


/*

The code is ugly, but that's just olympic programming in a nutshell.
And the idea is to maintain the length of the longest common
subsequence of every one of seq1 and seq2's prefixes.

*/
