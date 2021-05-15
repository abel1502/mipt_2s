/*

Вам дан неориентированный граф, состоящий из n вершин. На каждой вершине записано число; число, записанное на вершине i, равно ai. Изначально в графе нет ни одного ребра.

Вы можете добавлять ребра в граф за определенную стоимость. За добавление ребра между вершинами x и y надо заплатить ax+ay монет. Также существует m специальных предложений, каждое из которых характеризуется тремя числами x, y и w, и означает, что можно добавить ребро между вершинами x и y за w монет. Эти специальные предложения не обязательно использовать: если существует такая пара вершин x и y, такая, что для нее существует специальное предложение, можно все равно добавить ребро между ними за ax+ay монет.

Сколько монет минимально вам потребуется, чтобы сделать граф связным? Граф является связным, если от каждой вершины можно добраться до любой другой вершины, используя только ребра этого графа.

Входные данные
В первой строке заданы два целых числа n и m (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}2\N{DOT OPERATOR}105, 0\N{LESS-THAN OR EQUAL TO}m\N{LESS-THAN OR EQUAL TO}2\N{DOT OPERATOR}105) — количество вершин в графе и специальных предложений, соответственно.

Во второй строке заданы n целых чисел a1,a2,…,an (1\N{LESS-THAN OR EQUAL TO}ai\N{LESS-THAN OR EQUAL TO}1012) — числа, записанные на вершинах.

Затем следуют m строк, в каждой из которых заданы три целых числа x, y и w (1\N{LESS-THAN OR EQUAL TO}x,y\N{LESS-THAN OR EQUAL TO}n, 1\N{LESS-THAN OR EQUAL TO}w\N{LESS-THAN OR EQUAL TO}1012, x\N{NOT EQUAL TO}y), обозначающие спецпредложение: можно добавить ребро между вершинами x и y за w монет.

Выходные данные
Выведите одно целое число — минимальное количество монет, которое необходимо потратить, чтобы сделать граф связным.

Примеры
входные данные
3 2
1 3 3
2 3 5
2 1 1
выходные данные
5
входные данные
4 0
1 3 3 7
выходные данные
16
входные данные
5 4
1 2 3 4 5
1 2 8
1 3 10
1 4 7
1 5 15
выходные данные
18
Примечание
В первом примере из условия можно соединить 1 и 2 при помощи 2-го спецпредложения, а затем 1 и 3 без использования спецпредложения.

В следующих двух примерах оптимальный ответ можно получить без использования спецпредложений.

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


