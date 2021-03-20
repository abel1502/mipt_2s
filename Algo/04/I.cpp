/*

В компьютерной сети вашей фирмы n компьютеров. В последнее время свитч, к которому они подключены, сильно барахлит, и потому не любые два компьютера могут связаться друг с другом. Кроме того, если компьютер a обменивается информацией с компьютером b, то никакие другие компьютеры не могут в это время обмениваться информацией ни с a, ни c b. Вам необходимо вычислить максимальное количество компьютеров, которые могут одновременно участвовать в процессе обмена информацией.

Входные данные
В первой строке файла задано число n (1\N{LESS-THAN OR SLANTED EQUAL TO}n\N{LESS-THAN OR SLANTED EQUAL TO}18). Далее идут n строк по n символов, причём j-й символ i-й строки равен 'Y', если i-й и j-й компьютеры могут обмениваться информацией, иначе он равен 'N'. Верно, что i-й символ i-й строки всегда равен 'N' и, кроме того, матрица символов симметрична.

Выходные данные
Выведите максимальное количество компьютеров, которые могут одновременно участвовать в процессе обмена информацией.

Пример
входные данные
5
NYYYY
YNNNN
YNNNY
YNNNY
YNYYN
выходные данные
4

*/

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <vector>


int main() {
    unsigned n = 0;
    int res = scanf("%u", &n);
    assert(res == 1);

    std::vector<std::vector<bool>> conn(n, std::vector<bool>(n, 0));

    for (unsigned i = 0; i < n; ++i) {
        scanf("\n");

        for (unsigned j = 0; j < n; ++j) {
            char tmp = 0;
            res = scanf("%c", &tmp);
            assert(res == 1);

            conn[i][j] = (tmp == 'Y');
        }
    }


    std::vector<unsigned> dp(1 << n, 0);

    for (unsigned mask = 0; mask < (unsigned)(1 << n); ++mask) {
        for (unsigned from = 0; from < n; ++from) {
            if ((mask >> from) & 1) continue;

            for (unsigned to = 0; to < n; ++to) {
                if ((mask >> to) & 1 || !conn[from][to]) continue;

                unsigned newMask = mask | (1 << from) | (1 << to);

                dp[newMask] = std::max(dp[newMask], dp[mask] + 1);
            }
        }
    }

    unsigned bestScore = 0;

    for (unsigned mask = 0; mask < (unsigned)(1 << n); ++mask) {
        bestScore = std::max(bestScore, dp[mask]);
    }

    printf("%u\n", bestScore << 1);

    return 0;
}

/*

dp[mask] - the maximum possible number of computer pairs talking among the masked subset (with all of them engaged)
Time complexity is O(2**n*n**2) (again)

*/
