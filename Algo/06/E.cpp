/*

��� ����� �� ���� ������������� ���� �����������!

� ������ ���� ��������� ���������� ������. � ������ ������ ������ �������, ��� ������ �������� ���������������� ������ �� ���� ������� � ����� ����������� �������� ������ � ����. ��������, ���������� ���������, ������������������ �� ���������� ����� �����. � ������ ��������� � ���� �������� ����: ���� ������ A �� B � ��� ���������� ������ ������ B, ���������� �� 1 ������� ������ A. ����� � ������ ��������� ���� �������� � �����, ������� �� ������ ���������, ����� ����������� ��������. �������� ��������� � ��� ������, ������� ������ ������.

��������, ���� �� ������ �������� 100 �������� ��� �� ������� ����� � ���������, ��� ���� ������ ����� 29.75, � �������� ����� 0.39, �� �������� (100\N{MINUS SIGN}0.39)\N{DOT OPERATOR}29.75=2963.3975 ������ (��, ���� �������).

�� ����� ������, ��� � ������ ������������ ����� N �����. ����������� �� ������� 1,2,�,N. ����� ������ �������� ������������ 6 �����: ����� A � B � ������ ������������ �����, � ����� ������������ RAB,CAB,RBA � CBA � �������� ����� � �������� ��� �������� �� A � B � �� B � A ��������������.

��� �������� ��������� ������ � ������ S. �� ������� ��������, ����� ��, ����� ���������� �������� ������ ��������� ���� �������. �������, �� �����, ����� � ����� ��� ������ ����� ���� � ������ S. �������� ��� �������� �� ��� ������. ��� ������ ������ ������ ����� ��������������� ����� �����.

������� ������
������ ������ �������� ������ �����: N � ���������� �����, M � ���������� ����������, S � ������, ������� ����������� ���, � V � ���������� ������ ���� ������. ��������� M ����� �������� �� 6 �����, ������� ������ �������� ��������������� ���������� � ��������� ���� �������. ����� ����������� ����� ��� ����������� ���������. ����������� ����������� 1\N{LESS-THAN OR SLANTED EQUAL TO}S\N{LESS-THAN OR SLANTED EQUAL TO}N\N{LESS-THAN OR SLANTED EQUAL TO}100,1\N{LESS-THAN OR SLANTED EQUAL TO}M\N{LESS-THAN OR SLANTED EQUAL TO}100, V � ������������ �����, 0\N{LESS-THAN OR SLANTED EQUAL TO}V\N{LESS-THAN OR SLANTED EQUAL TO}103.

� ������ ��������� ����� ������ � �������� � ������������ �����, ���������� � ��������� �� ���� ���� ������ ����� ������� � ���������� �������������, ������ 10\N{MINUS SIGN}2\N{LESS-THAN OR SLANTED EQUAL TO}rate\N{LESS-THAN OR SLANTED EQUAL TO}102,0\N{LESS-THAN OR SLANTED EQUAL TO}comission\N{LESS-THAN OR SLANTED EQUAL TO}102.

������ ������������������ �������� ������ �������, ���� ������ �������� ������������ � ��� �� ����� ������ ����. �� ������ �������, ��� ��������� �������� ����� ���� � �������� �� ����� ������������ 104 ��� ����� ������� ������������������ ��������.

�������� ������
���� ��� ����� ��������� ���� �������, �������� YES, ����� �������� NO.

������
������� ������
3 2 1 10.0
1 2 1.0 1.0 1.0 1.0
2 3 1.1 1.0 1.1 1.0
�������� ������
NO

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


struct GraphState {
    static constexpr long double INF = 0.;

    struct NodeState {
        long double dist;
        bool used;
        bool negLoop;

        NodeState() :
            dist{INF},
            used{false},
            negLoop{false} {}

        // I guess rule of 5 is actually implicitly implemented for this

        inline void enter() {
            used = true;
        }

        inline void exit() {
        }
    };

    struct Edge {
        unsigned from;
        unsigned to;
        long double rate;
        long double fee;

        Edge() :
            from{-1u},
            to{-1u},
            rate{0.},
            fee{0.} {}

        Edge(unsigned new_from, unsigned new_to, long double new_rate, long double new_fee) :
            from{new_from},
            to{new_to},
            rate{new_rate},
            fee{new_fee} {}
    };

    std::vector<NodeState> graph;
    std::vector<Edge> edges;

    GraphState() :
        graph{} {}

    GraphState(unsigned nodeCnt) :
        graph(nodeCnt) {}

    inline void link(unsigned from, unsigned to, long double rate, long double fee) {
        edges.push_back(Edge{from, to, rate, fee});
    }

    bool fordBellman(unsigned start, long double initial) {
        graph[start].dist = initial;

        for (unsigned i = 0; i < graph.size() * 2; ++i)
            for (const Edge &edge : edges)
                if (graph[edge.from].dist != INF)
                    graph[edge.to].dist = std::max(graph[edge.to].dist, (graph[edge.from].dist - edge.fee) * edge.rate);

        for (unsigned i = 0; i < graph.size() - 1; ++i) {
            for (const Edge &edge : edges) {
                if (graph[edge.from].dist == INF)
                    continue;

                if ((graph[edge.from].dist - edge.fee) * edge.rate > graph[edge.to].dist) {
                    return true;
                }
            }
        }

        return graph[start].dist > initial;
    }
};


int main() {
    unsigned n = 0,
             m = 0,
             s = 0;
    long double v = 0.;
    int res = scanf("%u %u %u %Lg", &n, &m, &s, &v);
    assert(res == 4);

    GraphState gs{n};

    for (unsigned i = 0; i < m; ++i) {
        unsigned from = 0, to = 0;
        long double rate_ab = 0., rate_ba = 0., fee_ab = 0., fee_ba = 0.;

        res = scanf("%u %u %Lg %Lg %Lg %Lg", &from, &to, &rate_ab, &fee_ab, &rate_ba, &fee_ba);
        assert(res == 6);

        gs.link(from - 1, to - 1, rate_ab, fee_ab);
        gs.link(to - 1, from - 1, rate_ba, fee_ba);
    }

    if (gs.fordBellman(s - 1, v))
        printf("YES\n");
    else
        printf("NO\n");

    return 0;
}


/*

We just look for a positive loop from s with Ford-Bellman. Max told me regular doubles caused issues, so hello 80-bit floats)

*/


