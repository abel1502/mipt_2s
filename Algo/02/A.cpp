/*

������ ��� ����������� ��������� � ����� ��������� ������ �����. �� ��������� ���� ������ �� n ������ � m �������. �� ���� ������, �� ������ ����� ������� �����. ��� ����� ������ � ��� �������� ������, ������� ��� ������ ����� �� �����. ��� ���������, ��� ��� ������� ���� ����� �� ����� � ��� �� �����, � ���� ���������� �� ��� �� ����� �����. ����� ���� ���� ���������� ����� � ������ �����, �.�. ���� ����� � ���� � �� �� �������.

����� ������� ����� ���������� ����� �����.

� ���� ����� ���������� 4 ��������. ������� ������ �� ���� ���� ��������, ��� ����� ������ ����, ��������� ��������� ���������� �� ������. ������ ����� � 1 �.�. �� ��������. ������� �������� �� ��� ������, ������� ������� ���� ������, ��� � ������ ����� ������ ������� ���� �� ���� ����� (�� �� �����������, ����� �� ���� � ����� �� ������ �����, �.�. ����� ����� �� ���������).

������� ������ ��� ������� ����� ����������� �� ������ � ����� � ����� ����� �������, ��� ��������� ��������� ���� ����� ����� ����������.

������� ������
������ ������ �������� ����� n (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}2011) � ���������� ������. ��������� n ����� �������� ����� ������ � ���������� �� ������� ����� �� �� ����� (� ����������). ����� ������� ����� m � m �����, ����������� ������� � ��� �� ������� (0\N{LESS-THAN OR EQUAL TO}m\N{LESS-THAN OR EQUAL TO}2011, m\N{LESS-THAN OR EQUAL TO}3n).

����� ������� �� ���� ����������� ��������, ������ ����� ���������, ��������� ���������. ������ ��� ����� ����� �� 1 �� 15 ��������. ������� ��� ����� �� ���������. ���������� � ��������������� ����� �����, ��������������� 104.

�������� ������
�� ������ ������ �������� ���������� ��������� ���������� ����� (�.�), ������� �������� ��������� �� �����. �� ������ ������ �������� ����� k � ���������� �����, ������� ����� ��������. � ��������� k ������� ��������, ��� ������ � ��������������� �����. ������� ������ ������� � �������� ���. �� ������ �������� � �����, � ����� � ������������ �������. ���� ����� �� ���������� � ������ ������� �����.

�������
������� ������
2
Anton 5
Maxim 10
5
Anna 1
Maria 12
Tanya 10
Elena 8
Marina 6
�������� ������
18
2
Taxi 1: Anton, Marina and Anna.
Taxi 2: Maxim, Maria, Tanya and Elena.
������� ������
1
Romeo 100
1
Juliet 200
�������� ������
200
1
Taxi 1: Romeo and Juliet.
������� ������
1
Jack 17
0
�������� ������
17
1
Taxi 1: Jack.

*/


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <vector>


struct Nameholder {  // An object possessing a proper name and a digital destination (i.e. a boy or a girl)
    char name[16];
    unsigned dest;

    bool operator<(const Nameholder &other) const {
        return dest < other.dest;
    }
};


struct Item {

    static const int INF = INT_MAX;

    unsigned cost;
    unsigned taxis;
    unsigned girlsCnt;
    unsigned boysCnt;

    Item() :
        cost{INF},
        taxis{0},
        girlsCnt{0},
        boysCnt{0} {}

    Item(unsigned new_cost, unsigned new_taxis, unsigned new_girlsCnt, unsigned new_boysCnt) :
        cost{new_cost},
        taxis{new_taxis},
        girlsCnt{new_girlsCnt},
        boysCnt{new_boysCnt} {}
};


int main() {
    unsigned n = 0;
    unsigned m = 0;

    int res = scanf("%u", &n);
    assert(res == 1);

    std::vector<Nameholder> boys(n + 1);
    strcpy(boys[0].name, "<fake boy>");
    boys[0].dest = 0;
    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%15s %u", boys[i].name, &boys[i].dest);
        assert(res == 2);
    }

    res = scanf("%u", &m);
    assert(res == 1);

    std::vector<Nameholder> girls(m + 1);
    strcpy(girls[0].name, "<fake girl>");
    girls[0].dest = 0;
    for (unsigned i = 0; i < m; ++i) {
        res = scanf("%15s %u", girls[i].name, &girls[i].dest);
        assert(res == 2);
    }

    std::sort(boys.begin(), boys.end());
    std::sort(girls.begin(), girls.end());


    std::vector<std::vector<Item>> dp(m + 1, std::vector<Item>(n + 1, Item{}));
    dp[0][0].cost = 0;

    for (unsigned i = 0; i <= m; ++i) {
        for (unsigned j = 1; j <= n; ++j) {
            for (unsigned boysCnt = 1; boysCnt <= 4; ++boysCnt) {
                for (unsigned girlsCnt = 0; girlsCnt + boysCnt <= 4; ++girlsCnt) {
                    if (boysCnt > j || girlsCnt > i)
                        break;

                    unsigned cost = dp[i - girlsCnt][j - boysCnt].cost + std::max(boys[j].dest, girls[i].dest);
                    if (dp[i][j].cost > cost)
                        dp[i][j] = {cost, dp[i - girlsCnt][j - boysCnt].taxis + 1, girlsCnt, boysCnt};
                }
            }
        }
    }

    /*for (unsigned i = 0; i <= m; ++i) {
        for (unsigned j = 0; j <= n; ++j) {
            printf("<");

            if (dp[i][j].cost == Item::INF)
                printf("INF");
            else
                printf("%u", dp[i][j].cost);

            printf(" %u> ", dp[i][j].taxis);
        }

        printf("\n");
    }*/

    printf("%u\n%u\n", dp[m][n].cost, dp[m][n].taxis);

    unsigned boy = n;
    unsigned girl = m;

    for (unsigned taxi = 1; taxi <= dp[m][n].taxis; ++taxi) {
        printf("Taxi %u: ", taxi);

        unsigned boysCnt = dp[girl][boy].boysCnt;
        unsigned girlsCnt = dp[girl][boy].girlsCnt;
        unsigned cnt = boysCnt + girlsCnt;

        for (unsigned i = girl; i > girl - girlsCnt; --i) {
            printf("%s", girls[i].name);

            if (--cnt != 1)
                printf(", ");
        }

        for (unsigned i = boy; i > boy - boysCnt + 1; --i) {
            printf("%s", boys[i].name);

            if (--cnt != 1)
                printf(", ");
        }

        if (girlsCnt + boysCnt != 1) {
            printf(" and ");
        }

        printf("%s.\n", boys[boy - boysCnt + 1].name);

        boy -= boysCnt;
        girl -= girlsCnt;
    }

    return 0;
}

/*

dp[i][j] - the lowest cost and number of taxis sufficient for the first i girls and j boys
// To be honest, the core idea isn't mine - we discussed it withing the group. This seems like the hardest task of the contest

*/

