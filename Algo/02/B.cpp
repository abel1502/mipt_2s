/*

��� ��������� �������� ���������, ������� �� �������� ������������������ ������� ������������ �������������� � ��������������������� (�.� ����� ������������������ ����� ai1,ai2,�,aik (i1<i2<�<ik), ��� ai1\N{GREATER-THAN OR EQUAL TO}ai2\N{GREATER-THAN OR EQUAL TO}�\N{GREATER-THAN OR EQUAL TO}aik � �� ���������� ������������������ � ���� �� ���������� ������ k+1).

������� ������
� ������ ������ ������ ����� n � ���������� ��������� ������������������ (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}239017). � ����������� ������� ���� ���� ����� ������������������ ai, ���������� ���� �� ����� ������������ ����������� �������� � ��������� ������ (��� ����� �� ����������� �� ������ 231\N{MINUS SIGN}2).

�������� ������
��� ���������� ������ � ������ ������ ��������� ����� ����� k � ����� ������������ �������������� ���������������������. � ����������� ������� ������ ���� �������� (�� ������ ����� � ������ ������) ��� ������ ��������� �������� ������������������ ij, ���������� ������� ���������������������. ������ ��������� � ������� �����������. ���� ����������� ������� ���������, ����������� �������� �����.

������
������� ������
5
5 8
10 4 1
�������� ������
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
