/*

�������� BrokenTiles ��������� �������� ������������� �� ������ � ������������� �������� ���� �� ������ � ����� ������, ������ �� ������� ����� ������ 1\N{MULTIPLICATION SIGN}1 ����. ��������, ��� ����� ���� ������������� ����� ����� �������� ������ �� ������� ����� �������������� M\N{MULTIPLICATION SIGN}N ������.

������ ��� ����������� ����������� ����� � ��������� ���� ����������� ��������� ����� ��� ��������� ��������: �� ������, ������ ����� ������ �������� �������, ����� ����, ���������� � ���� �� �����, ��������� �� ������ ���� ��������� �������� ���� �����, � �� ������, ���� ���� ������ ���� �����������.

��� �������� ������������, ���� �������� �����������, ���� � ��� ����� �� ����������� �������� 2\N{MULTIPLICATION SIGN}2 �����, ��������� ��������� �������� ������ �����.

��� ����������� ����������� ����� ��������� ���������� ������, ������� �������� �� ������ ���������, ������ ��� ����������� ����� ������� ������� ����������. �������� ���!

������� ������
�� ������ ������ �������� ����� ��������� ��� ������������� ����� �����, ����������� �������� � M � N (1\N{LESS-THAN OR SLANTED EQUAL TO}M\N{MULTIPLICATION SIGN}N\N{LESS-THAN OR SLANTED EQUAL TO}30).

�������� ������
�������� � �������� ���� ������������ ����� � ���������� ��������� ����������� ������, ������� ����� �������� �� ����� ������� M\N{MULTIPLICATION SIGN}N. �����, ������������ ���� �� ����� �������, ��������� ��� ���������� ��������� ����������.

�������
������� ������
1 1
�������� ������
2
������� ������
1 2
�������� ������
4

*/

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <string>
#include <algorithm>
#include <vector>
#include <numeric>


int main() {
    unsigned n = 0;
    unsigned m = 0;

    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    if (n > m)
        std::swap(n, m);

    std::vector<std::vector<unsigned long long>> dp(m, std::vector<unsigned long long>(1 << n, 0));
    for (unsigned mask = 0; mask < (unsigned)1 << n; ++mask) {
        dp[0][mask] = 1;
    }

    std::vector<std::vector<bool>> transition(1 << n, std::vector<bool>(1 << n));
    for (unsigned mask1 = 0; mask1 < (unsigned)1 << n; ++mask1) {
        for (unsigned mask2 = 0; mask2 < (unsigned)1 << n; ++mask2) {
            transition[mask1][mask2] = true;

            for (unsigned k = 0; k < n - 1; ++k) {
                unsigned sqrState = ((mask1 >> k) & 0b11) << 2 | ((mask2 >> k) & 0b11);

                if (sqrState == 0b0000 || sqrState == 0b1111) {
                    transition[mask1][mask2] = false;
                    break;
                }
            }

            //if (transition[mask1][mask2])
            //    printf("> %u%u -> %u%u\n", (mask1 >> 1) & 1, (mask1 >> 0) & 1, (mask2 >> 1) & 1, (mask2 >> 0) & 1);
        }
    }

    for (unsigned i = 0; i < m - 1; ++i) {
        for (unsigned mask1 = 0; mask1 < (unsigned)1 << n; ++mask1) {
            for (unsigned mask2 = 0; mask2 < (unsigned)1 << n; ++mask2) {
                dp[i + 1][mask2] += transition[mask1][mask2] ? dp[i][mask1] : 0;
            }
        }
    }

    printf("%llu\n", std::accumulate(dp[m - 1].begin(), dp[m - 1].end(), 0ULL));

    return 0;
}

/*

dp[i][mask] - the number of valid colorings of an n*(i+1) floor with the leftmost column being colored according to the mask

*/


