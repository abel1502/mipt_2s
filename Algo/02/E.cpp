/*

������ ����������� �������� ��� ����� ������������ �������. ���������� ��� ����� ����� ������������ �������, ������ ������, ��������, ������ ����������� ���������� �������: ���� ���� �� ����� �� ����� ��� � ��� ���� ����������� ������, �� ������� ����� ������ ���������; � ��������� ������ �������� ����� ��� ����, ��� � ������ �����.

��� ���������� ������������, ���� ��� ��������� ������������. ��������� ����������� ����� ���������� ������, ���� ��� ����� ���� ��������� ����� �� ����� ��������� �����������. ����� ��������� ���������� ����� ����� � ��. ������� ���������� ������ �������� ����������� ����� ���� n?

������� ������
� ������ ������ �������� ����� ������ ����� n (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}2000).

�������� ������
� ������ ������ ��������� ����� �������� ���� ����� � ���������� ������ �������� ����������� ����� ���� n.

�������
������� ������
2
�������� ������
1
������� ������
5
�������� ������
2

*/


#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>


int main() {
    typedef unsigned long long val_t;

    unsigned n = 0;
    int res = scanf("%u", &n);
    assert(res == 1);

    std::vector<std::vector<val_t>> dp(n + 1, std::vector<val_t>(n + 1, 0));
    dp[0][0] = 1;

    for (unsigned power = 1; power <= n; ++power) {
        for (unsigned max = 0; max <= power; ++max) {
            for (unsigned other = 0; other * 2 <= max; ++other) {
                dp[power][max] += dp[power - max][other];
            }
        }
    }

    val_t sum = 0;

    for (unsigned i = 0; i <= n; ++i) {
        sum += dp[n][i];
    }

    printf("%llu\n", sum);

    return 0;
}


/*

dp[power][max] holds the number of peaceful sets of `power` power and with `max` as max.
My groupmates shared the incredibly valuable info that O(n**3) actually works - so why not?

*/


