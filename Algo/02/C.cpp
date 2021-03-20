/*

Вдоль прямой дороги расположены деревни. Дорога представляется целочисленной осью, а расположение каждой деревни задается одним целым числом — координатой на этой оси. Никакие две деревни не имеют одинаковых координат. Расстояние между двумя деревнями вычисляется как модуль разности их координат.

В некоторых, не обязательно во всех, деревнях будут построены почтовые отделения. Деревня и расположенное в ней почтовое отделение имеют одинаковые координаты. Почтовые отделения необходимо расположить в деревнях таким образом, чтобы общая сумма расстояний от каждой деревни до ближайшего к ней почтового отделения была минимальной.

Входные данные
В первой строке содержатся два целых числа: количество деревень n (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}300) и количество почтовых отделений m (1\N{LESS-THAN OR EQUAL TO}m\N{LESS-THAN OR EQUAL TO}30), m\N{LESS-THAN OR EQUAL TO}n. Вторая строка содержит n целых чисел в возрастающем порядке, являющихся координатами деревень. Для каждой координаты x верно 1\N{LESS-THAN OR EQUAL TO}x\N{LESS-THAN OR EQUAL TO}104.

Выходные данные
Первая строка выходного файла должна содержать одно целое число — общую сумму расстояний от каждой деревни до её ближайшего почтового отделения. Вторая строка должна содержать m целых чисел в возрастающем порядке. Эти числа являются искомыми координатами почтовых отделений. Если для заданного расположения деревень есть несколько решений, необходимо найти любое из них.

Пример
входные данные
10 5
1 2 3 6 7 9 11 22 44 50
выходные данные
9
2 9 22 44 50

*/

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <vector>


struct Item {

    static const int INF = INT_MAX;
    static const unsigned BAD_IND = (unsigned)-1;

    unsigned score;
    unsigned parentVillage;
    unsigned parentPost;

    Item() :
        score{INF},
        parentVillage{BAD_IND},
        parentPost{BAD_IND} {}

    Item(unsigned new_score) :
        score{new_score},
        parentVillage{BAD_IND},
        parentPost{BAD_IND} {}

    Item(unsigned new_score, unsigned new_parentVillage, unsigned new_parentPost) :
        score{new_score},
        parentVillage{new_parentVillage},
        parentPost{new_parentPost} {}
};


int main() {
    unsigned n = 0;
    unsigned m = 0;

    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    std::vector<unsigned> villages(n);
    std::vector<std::vector<Item>> dp(m + 1, std::vector<Item>(n + 1, Item{}));

    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%u", &villages[i]);
        assert(res == 1);
    }

    for (unsigned i = 0; i < n; ++i) {
        unsigned sum = 0;

        for (unsigned j = 0; j < n; ++j) {
            sum += std::abs((int)(villages[i] - villages[j]));
        }

        dp[1][i].score = sum;
    }

    for (unsigned i = 1; i < m; ++i) {
        for (unsigned j = i; j < n; ++j) {
            dp[i + 1][j] = Item{};

            for (unsigned k = j - 1; k >= i; --k) {
                unsigned score = dp[i][k].score;

                for (unsigned l = 0; l < n; ++l) {
                    unsigned newDist = std::abs((int)(villages[l] - villages[j]));
                    unsigned oldDist = std::abs((int)(villages[l] - villages[k]));

                    if (newDist < oldDist)
                        score -= oldDist - newDist;
                }

                if (score < dp[i + 1][j].score)
                    dp[i + 1][j] = {score, i, k};
            }
        }
    }

    Item min{};
    unsigned minInd = Item::BAD_IND;

    for (unsigned i = 0; i < n; ++i) {
        if (dp[m][i].score < min.score) {
            min = dp[m][i];
            minInd = i;
        }
    }

    printf("%u\n", min.score);

    std::vector<unsigned> ans{villages[minInd]};

    while (min.parentVillage != Item::BAD_IND) {
        ans.push_back(villages[min.parentPost]);

        min = dp[min.parentVillage][min.parentPost];
    }

    for (unsigned i = ans.size(); i > 0; --i) {
        printf("%u ", ans[i - 1]);
    }
    printf("\n");

    return 0;
}

/*

dp[i][j] - the best score for a distribution of i post offices over first j + 1 villages with the last post in the jth one

*/

