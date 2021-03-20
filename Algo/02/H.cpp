/*

���� ��� ������������������ ����� � a � b. ����� ����� ���������� ����� ������������ ���������������������. ����� ���������: ����� 1\N{LESS-THAN OR EQUAL TO}i1<i2<\N{MIDLINE HORIZONTAL ELLIPSIS}<ik\N{LESS-THAN OR EQUAL TO}a.n � 1\N{LESS-THAN OR EQUAL TO}j1<j2\N{MIDLINE HORIZONTAL ELLIPSIS}<jk\N{LESS-THAN OR EQUAL TO}b.n, ��� \N{FOR ALL}t:ait=bjt,ait<ait+1 � k �����������.

������� ������
�� ������ ������ ����� ����� n � m �� 1 �� 3000 � ����� �������������������. ������ ������ �������� n ����� �����, �������� ������ ������������������. ������ ������ �������� m ����� �����, �������� ������ ������������������. ��� �������� ������������������� � ����� ��������������� �����, �� ������������� 109.

�������� ������
�������� ���� ����� ����� � ����� ���������� ����� ������������ ���������������������.

������
������� ������
6 5
1 2 1 2 1 3
2 1 3 2 1
�������� ������
2

*/

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <vector>


int main() {
    unsigned n = 0;
    unsigned m = 0;

    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    std::vector<unsigned> a(n);
    std::vector<unsigned> b(m);
    std::vector<std::vector<unsigned>> dp(n + 1, std::vector<unsigned>(m + 1, 0));

    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%u", &a[i]);
        assert(res == 1);
    }

    for (unsigned i = 0; i < m; ++i) {
        res = scanf("%u", &b[i]);
        assert(res == 1);
    }

    unsigned ans = 0;

    for (unsigned i = 0; i < n; ++i) {
        unsigned max = 0;

        for (unsigned j = 0; j < m; ++j) {
            dp[i + 1][j + 1] = dp[i][j + 1];

            if (a[i] == b[j])
                dp[i + 1][j + 1] = std::max(dp[i + 1][j + 1], max + 1);
            else if (a[i] > b[j])
                max = std::max(max, dp[i][j + 1]);

            ans = std::max(ans, dp[i + 1][j + 1]);
        }
    }

    printf("%u\n", ans);

    return 0;
}

/*

dp[i][j] - highest length of the desired subsequence of a[0:i] and b[0:j].
I'm too lazy to optimize it to O(j) memory, but it's quite easy

*/


