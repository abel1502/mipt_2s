/*

Требуется найти в связном графе остовное дерево минимального веса.

Воспользуйтесь алгоритмом Крускала.

Входные данные
Первая строка входного файла содержит два натуральных числа n и m — количество вершин и ребер графа соответственно. Следующие m строк содержат описание ребер по одному на строке. Ребро номер i описывается тремя натуральными числами bi, ei и wi — номера концов ребра и его вес соответственно (1\N{LESS-THAN OR EQUAL TO}bi,ei\N{LESS-THAN OR EQUAL TO}n, 0\N{LESS-THAN OR EQUAL TO}wi\N{LESS-THAN OR EQUAL TO}100000). n\N{LESS-THAN OR EQUAL TO}20000,m\N{LESS-THAN OR EQUAL TO}100000.

Граф является связным.

Выходные данные
Первая строка выходного файла должна содержать одно натуральное число — вес минимального остовного дерева.

Пример
входные данные
2 1
1 2 10986
выходные данные
10986

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
    unsigned weight;

    friend bool operator<(const Edge &a, const Edge &b) {
        return a.weight < b.weight;
    }
};


int main() {
    unsigned n = 0,
             m = 0;
    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    Dsu dsu{n};
    std::vector<Edge> edges{};

    for (unsigned i = 0; i < m; ++i) {
        unsigned b = 0,
                 e = 0,
                 w = 0;

        res = scanf("%u %u %u", &b, &e, &w);
        assert(res == 3);

        edges.push_back(Edge{b - 1, e - 1, w});
    }

    std::sort(edges.begin(), edges.end());

    unsigned ans = 0;
    for (const Edge &e : edges) {
        if (dsu.get(e.from) != dsu.get(e.to)) {
            dsu.unite(e.from, e.to);

            ans += e.weight;
        }
    }

    printf("%u\n", ans);

    return 0;
}


/*

Kruscal this time

*/

