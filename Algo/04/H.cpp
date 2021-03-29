/*

�� ����� ����� �������� �� ��� ����� ����, �� ������ ������� ������������ ����� ��� � �� ���� ������, ����� ������ ����� ���������� �� �����������, � ����� ���. ����� �� ����� ��������������� �� ����� ��������� ������.

��� ������ ������������ ����� ��������� ������������� ������� n\N{MULTIPLICATION SIGN}m, � ������� ������ ������ � ��������� �����. ����� ����� ���� ������� ����� �� �����, ���� �������� ���. ��� ���� ���� ������, � ������� ��� ������ � �� ���������� ����� �����, � ����� �� � ����� ��������� �� ��������, � ���� ��, � ������� ��� ������� � �� ����, � �� � ����� ������ �������� ��������.

�������� ��� ������ ������ ���� ��� � ����������, � ��� ������, ��� � ����� �������� ������� 2\N{MULTIPLICATION SIGN}2 ������ ���� ������� ���������� � ����� �������. ������ ����� ����� ������ ���������� ��������� ��������� ������������� ������� ���� ������ �� ���������� � �����.

������� ������
� ������ ������ �������� ����� ������ ��� ����� ����� n � m (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}15 � 1\N{LESS-THAN OR EQUAL TO}m\N{LESS-THAN OR EQUAL TO}100) � ������ ������. ����� ������� n ����� �� m �������� � ������, ��� ������ '+' ��������, ��� ��������������� ����� �� ����� ��������, ������ '�' � ��������������� ����� ����� �������� � ����� ������ � ������ '.' � ����� ����� ������, ��� ��� ������ � ���� �������.

�������� ������
�������� ���������� ��������� ��������� ������� ����� �� ��� ������ �� ������ 109+7
������
������� ������
3 2
+.
..
.-
�������� ������
2

*/


#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <string>
#include <algorithm>
#include <vector>
#include <numeric>


int main() {
    const unsigned long long MOD = 1000000007;

    unsigned n = 0;
    unsigned m = 0;
    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    std::vector<unsigned> invFamine(m, 0), noFamine(m, 0);

    for (unsigned i = 0; i < n; ++i) {
        scanf("\n");

        char tmp = 0;
        for (unsigned j = 0; j < m; ++j) {
            res = scanf("%c", &tmp);
            assert(res == 1);

            if (tmp == '+') {
                noFamine[j] |= 1 << i;
            } else if (tmp == '-') {
                invFamine[j] |= 1 << i;
            } else {
                assert(tmp == '.');
            }
        }
    }

    std::vector<unsigned> dp1(1 << n, 0);
    std::vector<unsigned> dp2(1 << n, 0);

    for (unsigned mask = 0; !(mask >> n); ++mask) {
        dp1[(mask | noFamine[0]) & ~invFamine[0]] = 1;
    }

    for (unsigned i = 1; i < m; ++i) {
        dp2.assign(1 << n, 0);

        for (unsigned maskProto = 0; !(maskProto >> n); ++maskProto) {
            unsigned mask = (maskProto | noFamine[i]) & ~invFamine[i];
            dp2[mask] = dp1[mask ^ ((1 << n) - 1)];

            if (!((mask ^ 0x55555555) & ((1 << n) - 1) && (mask ^ 0xaaaaaaaa) & ((1 << n) - 1))) {
                dp2[mask] = (dp2[mask] + dp1[mask]) % MOD;
            }
        }

        dp2.swap(dp1);
    }

    unsigned long long ans = 0;
    std::vector<bool> seen(1 << n, false);
    for (unsigned maskProto = 0; !(maskProto >> n); ++maskProto) {
        unsigned mask = (maskProto | noFamine[m - 1]) & ~invFamine[m - 1];

        if (seen[mask]) continue;

        seen[mask] = true;

        ans = (ans + dp1[mask]) % MOD;
    }

    printf("%llu\n", ans);

    return 0;
}

/*

dp[i][mask] is a profiled-dynamics holding the number of ways to possibly starve the affected cities.
The requirement for a transition is that either the new layer's mask is the inverse of the current one,
or that it's the same, but there are no adjacent cities of the same faminity

*/

