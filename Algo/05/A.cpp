/*

������� ����� u ���������� ����������� ����� e, ���� u �������� ����� �� ������ ����� e.

����������, ����� e ���������� ����������� ������� u, ���� ���� �� ������ e � ��� ������� u.

�������� ������������� ����� G=(V,E) ���������� ������������� ������� �� |V| ����� � |E| ��������, � ������� �� ����������� i-� ������ � j-�� ������� �������� �������, ���� ������� i ���������� ����� j, � ���� � ��������� ������.

��� ����������������� ����. �������� ��� ������� �������������.

������� ������
� ������ ������ �������� ����� ������ ����� N � M ����� ������ � ���������� ������ � ���� � �����, �������������� (1\N{LESS-THAN OR SLANTED EQUAL TO}N\N{LESS-THAN OR SLANTED EQUAL TO}100, 0\N{LESS-THAN OR SLANTED EQUAL TO}M\N{LESS-THAN OR SLANTED EQUAL TO}10000). ��������� M ����� �������� �� ��� ����� ui � vi ����� ������ (1\N{LESS-THAN OR SLANTED EQUAL TO}ui,vi\N{LESS-THAN OR SLANTED EQUAL TO}N); ������ ����� ������ ��������, ��� � ����� ���������� ����� ����� ��������� ui � vi. и��� ���������� � ��� �������, � ������� ��� ���� �� ������� �����, ������� � �������.

�������� ������
�������� � �������� ���� N �����, �� M ����� � ������. j-� ������� i-� ������ ������ ���� ����� �������, ���� ������� i ���������� ����� j, � ���� � ��������� ������. ���������� �������� �������� ������ ����� ��������.

������
������� ������
3 2
1 2
2 3
�������� ������
1 0
1 1
0 1

*/

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>
#include <algorithm>


int main() {
    unsigned n = 0, m = 0;
    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    std::vector<std::vector<bool>> incidence(n, std::vector<bool>(m, false));

    for (unsigned i = 0; i < m; ++i) {
        unsigned u = 0, v = 0;
        res = scanf("%u %u", &u, &v);
        assert(res == 2);

        incidence[u - 1][i] = incidence[v - 1][i] = true;
    }

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j = 0; j < m; ++j) {
            printf("%u ", (unsigned)incidence[i][j]);
        }

        printf("\n");
    }

    return 0;
}


/*

Do I really need to explain anything?

*/
