/*

В своих странствиях Геральд наткнулся на реликтовую карту лучника Northern Realms. Эта карта применяется на карту мечника противника, снимает у него a очков атаки, а у соседей этой карты (слева и справа) снимается по b (1\N{LESS-THAN OR EQUAL TO}b<a\N{LESS-THAN OR EQUAL TO}10) очков атаки. Как только у карты противника очков атаки становится меньше нуля, она замораживается. Однако, эту карту нельзя применять на крайние карты (если нумеровать слева направо, то на карты с номерами 1 и n).

Геральду стало интересно, какое наименьшее количество реликтовых карт необходимо, чтобы заморозить всех мечников противника?

Для каждого мечника известно его количество очков атаки. Реликтовая карта лучника может применяться даже на замороженные карты.

Входные данные
В первой строке записано три целых числа n,a,b (3\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}10; 1\N{LESS-THAN OR EQUAL TO}b<a\N{LESS-THAN OR EQUAL TO}10). Вторая строка содержит последовательность n целых чисел — h1,h2,…,hn (1\N{LESS-THAN OR EQUAL TO}hi\N{LESS-THAN OR EQUAL TO}15), hi обозначает количество очков атаки i-го мечника.

Выходные данные
В первую строку выведите t — искомое минимальное количество реликтовых карт.

Во вторую строку выведите t чисел — номера мечников, на которые должны применяться реликтовые карты, чтобы убить всех мечников за t таких карт. Все выводимые номера должны находиться в отрезке от 2 до n\N{MINUS SIGN}1. Числа разделяйте пробелами. Если решений несколько, выведите любое. Числа выводите в любом порядке.

Примеры
входные данные
3 2 1
2 2 2
выходные данные
3
2 2 2
входные данные
4 3 1
1 4 1 1
выходные данные
4
2 2 3 3

*/


#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <string>
#include <algorithm>
#include <vector>
#include <numeric>
#include <math.h>


struct Item {
    static const int INF = 100000;
    static const unsigned BAD_IND = (unsigned)-1;

    unsigned hits;
    unsigned rightHits;
    unsigned offset;

    Item() :
        hits{INF},
        rightHits{INF},
        offset{BAD_IND} {}

    Item(unsigned new_hits, unsigned new_rightHits, unsigned new_offset) :
        hits{new_hits},
        rightHits{new_rightHits},
        offset{new_offset} {}
};


inline unsigned divRoundUp(unsigned a, unsigned b) {
    return (a + b - 1) / b;
}


int main() {
    unsigned n = 0;
    unsigned a = 0;
    unsigned b = 0;
    int res = scanf("%u %u %u", &n, &a, &b);
    assert(res == 3);

    std::vector<unsigned> h(n, 0);

    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%u", &h[i]);
        assert(res == 1);

        h[i]++;
    }

    using v1_t = std::vector<Item>;
    using v2_t = std::vector<v1_t>;
    using v3_t = std::vector<v2_t>;
    using v4_t = std::vector<v3_t>;

    v4_t dp(n, v3_t(17, v2_t(17, v1_t(17, Item{}))));

    for (unsigned attack3 = 0; attack3 <= h[0]; ++attack3) {
        for (unsigned attack2 = 0; attack2 <= h[1]; ++attack2) {
            for (unsigned attack1 = 0; attack1 <= h[2]; ++attack1) {
                unsigned hits = divRoundUp(attack3, b);
                hits = std::max(hits, divRoundUp(attack2, a));
                hits = std::max(hits, divRoundUp(attack1, b));

                //printf("> %u %u %u takes %u hits\n", h[0], attack2, attack1, hits);

                dp[2][attack3][attack2][attack1] = {hits, hits, 0};
            }
        }
    }

    for (unsigned i = 3; i < n; ++i) {
        for (unsigned attack3 = 0; attack3 <= h[i - 2]; ++attack3) {
            for (unsigned attack2 = 0; attack2 <= h[i - 1]; ++attack2) {
                for (unsigned attack1 = 0; attack1 <= h[i]; ++attack1) {

                    unsigned hits = divRoundUp(attack1, b);

                    int parAttack2 = std::max(0, (int)(attack3 - b * hits));
                    int parAttack1 = std::max(0, (int)(attack2 - a * hits));

                    unsigned oldHits = 0;
                    unsigned newHits = 0;

                    while (parAttack1 >= 0) {
                        oldHits = dp[i][attack3][attack2][attack1].hits;
                        newHits = dp[i - 1][h[i - 3]][std::max(0, parAttack2)][parAttack1].hits + hits;

                        if (newHits < oldHits) {
                            dp[i][attack3][attack2][attack1] = {newHits, hits, 1};
                        }

                        parAttack1 -= a;
                        parAttack2 -= b;
                        hits++;
                    }

                    if (i < 4) continue;

                    while (parAttack2 >= 0) {
                        oldHits = dp[i][attack3][attack2][attack1].hits;
                        newHits = dp[i - 2][h[i - 4]][h[i - 3]][parAttack2].hits + hits;

                        if (newHits < oldHits) {
                            dp[i][attack3][attack2][attack1] = {newHits, hits, 2};
                        }

                        oldHits = dp[i][attack3][attack2][attack1].hits;
                        newHits = dp[i - 1][h[i - 3]][parAttack2][0].hits + hits;

                        if (newHits < oldHits) {
                            dp[i][attack3][attack2][attack1] = {newHits, hits, 1};
                        }

                        parAttack1 -= a;
                        parAttack2 -= b;
                        hits++;
                    }

                    if (i < 5) continue;

                    oldHits = dp[i][attack3][attack2][attack1].hits;
                    newHits = dp[i - 3][h[i - 5]][h[i - 4]][h[i - 3]].hits + hits;

                    if (newHits < oldHits) {
                        dp[i][attack3][attack2][attack1] = {newHits, hits, 3};
                    }

                    oldHits = dp[i][attack3][attack2][attack1].hits;
                    newHits = dp[i - 2][h[i - 4]][h[i - 3]][0].hits + hits;

                    if (newHits < oldHits) {
                        dp[i][attack3][attack2][attack1] = {newHits, hits, 2};
                    }

                    oldHits = dp[i][attack3][attack2][attack1].hits;
                    newHits = dp[i - 1][h[i - 3]][0][0].hits + hits;

                    if (newHits < oldHits) {
                        dp[i][attack3][attack2][attack1] = {newHits, hits, 1};
                    }

                    /*
                    printf("> ... %u %u %u takes %u hits and turns into\n"
                           "  ... %u %u", h[i - 2], attack2, attack1, hits, parAttack2, parAttack1);
                    */
                }
            }
        }
    }

    unsigned counter = 0;

    unsigned attack3 = h[n - 3];
    unsigned attack2 = h[n - 2];
    unsigned attack1 = h[n - 1];

    /*#define LOG_DP_(I, A3, A2, A1, EXP) \
        printf(">> dp[%u][%u][%u][%u] = %u (%s), %u right hits, offset %u\n", I, A3, A2, A1, dp[I][A3][A2][A1].hits, dp[I][A3][A2][A1].hits == EXP ? "YES" : "NO", dp[I][A3][A2][A1].rightHits, dp[I][A3][A2][A1].offset);

    LOG_DP_(5, 3, 4, 0, 9);

    LOG_DP_(4, 4, 2, 0, 8);

    LOG_DP_(2, 7, 3, 4, 7);

    #undef LOG_DP_*/

    printf("%u\n", dp[n - 1][attack3][attack2][attack1].hits);

    for (unsigned i = n - 1; i > 1 && counter < 10000;) {
        unsigned rightHits = dp[i][attack3][attack2][attack1].rightHits;

        //printf("<%u> * %u, ", rightHits, i);

        for (unsigned j = 0; j < rightHits && counter < 10000; ++j) {
            printf("%u ", i - 1 + 1);
            counter++;
        }

        unsigned offset = dp[i][attack3][attack2][attack1].offset;
        unsigned new_i = i - offset;

        if (new_i >= i)
            break;

        if (offset == 1) {
            attack1 = std::max(0, (int)(attack2 - rightHits * a));
            attack2 = std::max(0, (int)(attack3 - rightHits * b));
            attack3 = h[i - 3];
        } else if (offset == 2) {
            attack1 = std::max(0, (int)(attack3 - rightHits * b));
            attack2 = h[i - 3];
            attack3 = h[i - 4];
        } else {
            assert(offset == 3);

            attack1 = h[i - 3];
            attack2 = h[i - 4];
            attack3 = h[i - 5];
        }

        i = new_i;
    }

    assert(counter == dp[n - 1][h[n - 3]][h[n - 2]][h[n - 1]].hits);

    printf("\n");

    return 0;
}

/*

dp[right][attack3][attack2][attack1] holds the minimal number of hits to kill [0, right] if the three guys on the right had attack3, attack2 and attack1 attack

*/
