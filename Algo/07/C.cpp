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


struct Dsu {
    struct Node {
        unsigned par = -1u;
        unsigned size = 1;
    };

    std::vector<Node> buf;

    Dsu(unsigned n) :
        buf(n) {

        for (unsigned i = 0; i < n; ++i) {
            buf[i].par = i;
        }
    }

    unsigned get(unsigned idx) {
        if (buf[idx].par == idx)
            return idx;

        return buf[idx].par = get(buf[idx].par);
    }

    void unite(unsigned a, unsigned b) {
        a = get(a);
        b = get(b);

        if (buf[a].size < buf[b].size)
            std::swap(a, b);

        buf[b].par = a;
        buf[a].size += buf[b].size;
    }
};


struct Edge {
    unsigned from;
    unsigned to;
    unsigned long long weight;

    friend bool operator<(const Edge &a, const Edge &b) {
        return a.weight < b.weight;
    }
};


static constexpr unsigned long long INF = 100000000000000000;


int main() {
    unsigned n = 0,
             m = 0;
    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    Dsu dsu{n};

    unsigned minIdx = -1u;
    unsigned long long min = INF;
    std::vector<unsigned long long> a(n);
    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%llu", &a[i]);
        assert(res == 1);

        if (a[i] < min) {
            min = a[i];
            minIdx = i;
        }
    }


    std::vector<Edge> edges{};

    for (unsigned i = 0; i < m; ++i) {
        unsigned x = 0,
                 y = 0;
        unsigned long long w = 0;

        res = scanf("%u %u %llu", &x, &y, &w);
        assert(res == 3);

        edges.push_back(Edge{x - 1, y - 1, w});
    }

    for (unsigned i = 0; i < n; ++i) {
        if (i != minIdx)
            edges.push_back(Edge{i, minIdx, min + a[i]});
    }

    std::sort(edges.begin(), edges.end());

    unsigned long long ans = 0;
    for (const Edge &e : edges) {
        if (dsu.get(e.from) != dsu.get(e.to)) {
            dsu.unite(e.from, e.to);

            ans += e.weight;
        }
    }

    printf("%llu\n", ans);

    return 0;
}


/*

Kruscal again

*/


