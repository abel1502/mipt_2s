/*

Продавец аквариумов для кошек хочет объехать n городов, посетив каждый из них ровно один раз. Помогите ему найти кратчайший путь.

Входные данные
Первая строка входного файла содержит натуральное число n (1\N{LESS-THAN OR SLANTED EQUAL TO}n\N{LESS-THAN OR SLANTED EQUAL TO}13) — количество городов. Следующие n строк содержат по n чисел — длины путей между городами.

В i-й строке j-е число — ai,j — это расстояние между городами i и j (0\N{LESS-THAN OR SLANTED EQUAL TO}ai,j\N{LESS-THAN OR SLANTED EQUAL TO}106; ai,j=aj,i; ai,i=0).

Выходные данные
В первой строке выходного файла выведите длину кратчайшего пути. Во второй строке выведите n чисел — порядок, в котором нужно посетить города.

Пример
входные данные
5
0 183 163 173 181
183 0 165 172 171
163 165 0 189 302
173 172 189 0 167
181 171 302 167 0
выходные данные
666
4 5 2 3 1
Примечание
Города нумеруются с 1 до n.

*/

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <vector>


struct Item {

    static const int INF = INT_MAX;
    static const unsigned BAD_IND = (unsigned)-1;

    unsigned length;
    unsigned parentI;
    unsigned parentMask;

    Item() :
        length{INF},
        parentI{BAD_IND},
        parentMask{0} {}


    Item(unsigned new_length, unsigned new_parentI, unsigned new_parentMask) :
        length{new_length},
        parentI{new_parentI},
        parentMask{new_parentMask} {}
};


int main() {
    unsigned n = 0;
    int res = scanf("%u", &n);
    assert(res == 1);

    std::vector<std::vector<unsigned>> distances(n, std::vector<unsigned>(n, 0));

    for (unsigned i = 0; i < n; ++i)
        for (unsigned j = 0; j < n; ++j) {
            res = scanf("%u", &distances[i][j]);
            assert(res == 1);
        }

    std::vector<std::vector<Item>> dp(n, std::vector<Item>(1 << n, Item{}));

    for (unsigned i = 0; i < n; ++i) {
        dp[i][1 << i].length = 0;
    }

    for (unsigned mask = 1; mask < (unsigned)(1 << n); ++mask) {
        for (unsigned from = 0; from < n; ++from) {
            for (unsigned to = 0; to < n; ++to) {
                if ((mask >> to) & 1) continue;

                unsigned newMask = mask | (1 << to);
                unsigned newLength = dp[from][mask].length + distances[from][to];
                if (dp[to][newMask].length > newLength) {
                    dp[to][newMask] = Item{newLength, from, mask};
                }
            }
        }
    }

    Item min{};
    unsigned minInd = Item::BAD_IND;
    unsigned minMask = (1 << n) - 1;
    for (unsigned i = 0; i < n; ++i) {
        if (dp[i][minMask].length < min.length) {
            min = dp[i][minMask];
            minInd = i;
        }
    }

    printf("%u\n", min.length);

    while (minInd != Item::BAD_IND) {
        printf("%u ", minInd + 1);

        min = dp[minInd][minMask];
        minInd = min.parentI;
        minMask = min.parentMask;
    }

    printf("\n");

    return 0;
}

/*

dp[i][mask] - the number of the shortest path starting in i and covering the mask.
The overall time complexity will be O(2**n*n**2) - ain't that beautiful?

*/
