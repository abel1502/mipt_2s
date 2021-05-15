/*

����� ���������� ���������� � �����, ����� ������ ���������� ��������� �����. � ������ ��� ���� ����� ����� ������ ����������������� ����. ��� ����� ������ ������ � ����� ����� � ������� ���� ��� ��� � ������������ ���� ������, ���� ���� �� ����� ��������� �������. ���� �� ����� ���� ��� ����� � ����� ��������������� ���.

���� ������������ ����� ���� �� n\N{MULTIPLICATION SIGN}m ������, ������ ��������� ������ ��� �������� ������. � ����� ���� ������ ������� ������� 1\N{MULTIPLICATION SIGN}2 (������� ����� ������������ � �������� 2\N{MULTIPLICATION SIGN}1), ��������� ������� �������� � ���� a ������, � 1\N{MULTIPLICATION SIGN}1, ��������� ������� �������� b ������. ��� ���������� ������� ��� ������ ������, ������ ������ ��, �� ���������� ������� ���� �� �����.

����������, ����� ����� ����������� ���������� ������ ���� � ��� ������ ���������� ���� ���� ������.

������� ������
������ ������ �������� ����� �������� 4 ����� ����� n, m, a, b (1\N{LESS-THAN OR EQUAL TO}n,m\N{LESS-THAN OR EQUAL TO}100, |a|\N{LESS-THAN OR EQUAL TO}1000, |b|\N{LESS-THAN OR EQUAL TO}1000). ������ �� ����������� n ����� �������� �� m ��������: ������  �.� (�����) ���������� ����� ������ �����, � ������ �*� (��������) � ������.

�������� ������
� �������� ���� �������� ���� ����� � ����������� ���������� ������, ����� ������� ���� ����� ������ ���������� ���� ����, ������ ������ ������ ����� (�� � ������ ��).

������
������� ������
2 3 3 2
.**
.*.
�������� ������
5

*/


/*

��� ��� ����������������� ����, ��������� �� n ������. �� ������ ������� �������� �����; �����, ���������� �� ������� i, ����� ai. ���������� � ����� ��� �� ������ �����.

�� ������ ��������� ����� � ���� �� ������������ ���������. �� ���������� ����� ����� ��������� x � y ���� ��������� ax+ay �����. ����� ���������� m ����������� �����������, ������ �� ������� ��������������� ����� ������� x, y � w, � ��������, ��� ����� �������� ����� ����� ��������� x � y �� w �����. ��� ����������� ����������� �� ����������� ������������: ���� ���������� ����� ���� ������ x � y, �����, ��� ��� ��� ���������� ����������� �����������, ����� ��� ����� �������� ����� ����� ���� �� ax+ay �����.

������� ����� ���������� ��� �����������, ����� ������� ���� �������? ���� �������� �������, ���� �� ������ ������� ����� ��������� �� ����� ������ �������, ��������� ������ ����� ����� �����.

������� ������
� ������ ������ ������ ��� ����� ����� n � m (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}2\N{DOT OPERATOR}105, 0\N{LESS-THAN OR EQUAL TO}m\N{LESS-THAN OR EQUAL TO}2\N{DOT OPERATOR}105) � ���������� ������ � ����� � ����������� �����������, ��������������.

�� ������ ������ ������ n ����� ����� a1,a2,�,an (1\N{LESS-THAN OR EQUAL TO}ai\N{LESS-THAN OR EQUAL TO}1012) � �����, ���������� �� ��������.

����� ������� m �����, � ������ �� ������� ������ ��� ����� ����� x, y � w (1\N{LESS-THAN OR EQUAL TO}x,y\N{LESS-THAN OR EQUAL TO}n, 1\N{LESS-THAN OR EQUAL TO}w\N{LESS-THAN OR EQUAL TO}1012, x\N{NOT EQUAL TO}y), ������������ ���������������: ����� �������� ����� ����� ��������� x � y �� w �����.

�������� ������
�������� ���� ����� ����� � ����������� ���������� �����, ������� ���������� ���������, ����� ������� ���� �������.

�������
������� ������
3 2
1 3 3
2 3 5
2 1 1
�������� ������
5
������� ������
4 0
1 3 3 7
�������� ������
16
������� ������
5 4
1 2 3 4 5
1 2 8
1 3 10
1 4 7
1 5 15
�������� ������
18
����������
� ������ ������� �� ������� ����� ��������� 1 � 2 ��� ������ 2-�� ���������������, � ����� 1 � 3 ��� ������������� ���������������.

� ��������� ���� �������� ����������� ����� ����� �������� ��� ������������� ���������������.

*/


#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>
#include <queue>
#include <algorithm>
#include <cctype>
#include <string>
#include <numeric>
#include <set>


struct GraphState {
    static constexpr unsigned INF = 1000000000;

    struct NodeState {
        std::vector<unsigned> children;
        bool used       = false;
        unsigned match  = -1u;
    };

    std::vector<NodeState> graph;

    GraphState() :
        graph{} {}

    GraphState(unsigned nodeCnt) :
        graph(nodeCnt) {}

    bool augment(unsigned from) {
        if (graph[from].used)
            return false;

        graph[from].used = true;

        for (unsigned to : graph[from].children) {
            if (graph[to].match == -1u || augment(graph[to].match)) {
                graph[to].match = from;

                return true;
            }
        }

        return false;
    }

    void koon(unsigned width) {
        for (unsigned i = 0; i < graph.size(); ++i) {
            if ((i % width + i / width) & 1 || !augment(i))
                continue;

            for (unsigned j = 0; j < graph.size(); ++j) {
                graph[j].used = false;
            }
        }
    }

    inline void link(unsigned from, unsigned to) {
        graph[from].children.push_back(to);
    }
};


int main() {
    unsigned n = 0,
             m = 0;
    int      a = 0,
             b = 0;
    int res = scanf("%u %u %d %d\n", &n, &m, &a, &b);
    assert(res == 4);

    std::vector<std::vector<bool>> bridge(n, std::vector<bool>(m));
    unsigned missing = 0;

    for (unsigned i = 0; i < n; ++i) {
        char cur = 0;

        for (unsigned j = 0; j < m; ++j) {
            res = scanf("%c", &cur);
            assert(res == 1);

            bridge[i][j] = cur == '.';
            missing += cur == '*';
        }

        scanf("%c", &cur);
    }

    if (a >= 2 * b) {
        printf("%d\n", missing * b);
        return 0;
    }

    GraphState gs{n * m};

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j = 0; j < m; ++j) {
            if (bridge[i][j])
                continue;

            unsigned idx = i * m + j;

            #define NEIGHBOUR_(NI, NJ) {                                                \
                if ((NI) < n && (NJ) < m && !bridge[NI][NJ])                            \
                    gs.link(idx, (NI) * m + (NJ));                                      \
            }

            NEIGHBOUR_(i + 1, j    );
            NEIGHBOUR_(i - 1, j    );
            NEIGHBOUR_(i,     j + 1);
            NEIGHBOUR_(i,     j - 1);

            #undef NEIGHBOUR_
        }
    }

    gs.koon(m);

    unsigned ans = 0;
    for (GraphState::NodeState &cur : gs.graph) {
        ans += cur.match != -1u;
    }

    printf("%d\n", missing * b - ans * (2 * b - a));

    return 0;
}


/*

Koon this time (2 by 1 planks are edges in the polar graph of cells, split by the parity of the diagonals)

*/


