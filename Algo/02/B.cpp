/*

Вам требуется написать программу, которая по заданной последовательности находит максимальную невозрастающую её подпоследовательность (т.е такую последовательность чисел ai1,ai2,…,aik (i1<i2<…<ik), что ai1\N{GREATER-THAN OR EQUAL TO}ai2\N{GREATER-THAN OR EQUAL TO}…\N{GREATER-THAN OR EQUAL TO}aik и не существует последовательности с теми же свойствами длиной k+1).

Входные данные
В первой строке задано число n — количество элементов последовательности (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}239017). В последующих строках идут сами числа последовательности ai, отделенные друг от друга произвольным количеством пробелов и переводов строки (все числа не превосходят по модулю 231\N{MINUS SIGN}2).

Выходные данные
Вам необходимо выдать в первой строке выходного файла число k — длину максимальной невозрастающей подпоследовательности. В последующих строках должны быть выведены (по одному числу в каждой строке) все номера элементов исходной последовательности ij, образующих искомую подпоследовательность. Номера выводятся в порядке возрастания. Если оптимальных решений несколько, разрешается выводить любое.

Пример
входные данные
5
5 8
10 4 1
выходные данные
3
1 4 5

*/

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <vector>


struct Item {

    static const int INF = INT_MAX;
    static const unsigned BAD_IND = (unsigned)-1;

    int value;
    unsigned index;
    unsigned parent;

    Item() :
        value{INF},
        index{BAD_IND},
        parent{BAD_IND} {}

    Item(int new_value) :
        value{new_value},
        index{BAD_IND},
        parent{BAD_IND} {}

    Item(int new_value, unsigned new_index) :
        value{new_value},
        index{new_index},
        parent{BAD_IND} {}

    bool operator<(const Item &other) const {
        return value < other.value;
    }

    bool operator==(const Item &other) const {
        return value == other.value;
    }

    bool operator>(const Item &other) const {
        return value > other.value;
    }
};


int main() {
    unsigned n = 0;

    int res = scanf("%u", &n);
    assert(res == 1);

    std::vector<int> sequence(n);
    std::vector<Item> dp(n + 1);

    unsigned maxLen = 0;

    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%d", &sequence[i]);
        assert(res == 1);

        sequence[i] = -sequence[i];
    }

    dp[0].value = -Item::INF;

    for (unsigned i = 0; i < n; ++i) {
        unsigned j = (unsigned)(std::upper_bound(dp.begin(), dp.end(), (const Item){sequence[i]}) - dp.begin());

        /*printf(">> ");
        for (unsigned k = 0; k < n; ++k) {
            printf("%d ", dp[k].value);
        }
        printf("\n");*/

        assert(j >= 1 && j < n + 1);

        if (dp[j - 1].value <= sequence[i] && sequence[i] <= dp[j].value) {
            dp[j].value = sequence[i];
            dp[j].index = i;
            dp[i].parent = dp[j - 1].index;

            maxLen = std::max(maxLen, j);
        }
    }

    printf("%u\n", maxLen);

    assert(maxLen > 0 || n == 0);

    std::vector<unsigned> answer{};
    unsigned cur = dp[maxLen].index;

    for (unsigned i = 0; cur != Item::BAD_IND; ++i) {
        answer.push_back(cur);
        cur = dp[cur].parent;
    }

    for (unsigned i = maxLen - 1; i != (unsigned)-1; --i) {
        printf("%u ", answer[i] + 1);
        //printf("(%d) ", -sequence[answer[i]]);
    }

    printf("\n");

    return 0;
}

/*

dp[k] - the smallest end to a non-decreasing sequence among the current prefix [0, i)

I accidentally wrote a program to find the longest non-decreasing sequence, so in order to flip it into a
non-increasing one I multiplied everything by -1

*/
