/*

� ������������ ���� ����� ����� n �����������. � ��������� ����� �����, � �������� ��� ����������, ������ ��������, � ������ �� ����� ��� ���������� ����� ��������� ���� � ������. ����� ����, ���� ��������� a ������������ ����������� � ����������� b, �� ������� ������ ���������� �� ����� � ��� ����� ������������ ����������� �� � a, �� c b. ��� ���������� ��������� ������������ ���������� �����������, ������� ����� ������������ ����������� � �������� ������ �����������.

������� ������
� ������ ������ ����� ������ ����� n (1\N{LESS-THAN OR SLANTED EQUAL TO}n\N{LESS-THAN OR SLANTED EQUAL TO}18). ����� ���� n ����� �� n ��������, ������ j-� ������ i-� ������ ����� 'Y', ���� i-� � j-� ���������� ����� ������������ �����������, ����� �� ����� 'N'. �����, ��� i-� ������ i-� ������ ������ ����� 'N' �, ����� ����, ������� �������� �����������.

�������� ������
�������� ������������ ���������� �����������, ������� ����� ������������ ����������� � �������� ������ �����������.

������
������� ������
5
NYYYY
YNNNN
YNNNY
YNNNY
YNYYN
�������� ������
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
