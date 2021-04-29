/*

Дан взвешенный неориентированный граф. Требуется найти вес минимального пути между двумя вершинами.

Входные данные
Первая строка входного файла содержит два натуральных числа n и m — количество вершин и ребер графа соответственно. Вторая строка входного файла содержит натуральные числа s и t — номера вершин, длину пути между которыми требуется найти (1\N{LESS-THAN OR EQUAL TO}s,t\N{LESS-THAN OR EQUAL TO}n, s\N{NOT EQUAL TO}t).

Следующие m строк содержат описание ребер по одному на строке. Ребро номер i описывается тремя натуральными числами bi, ei и wi — номера концов ребра и его вес соответственно (1\N{LESS-THAN OR EQUAL TO}bi,ei\N{LESS-THAN OR EQUAL TO}n, 0\N{LESS-THAN OR EQUAL TO}wi\N{LESS-THAN OR EQUAL TO}100).

n\N{LESS-THAN OR EQUAL TO}100000, m\N{LESS-THAN OR EQUAL TO}200000.

Выходные данные
Первая строка выходного файла должна содержать одно натуральное число — вес минимального пути между вершинами s и t.

Если путь из s в t не существует, выведите -1.

Пример
входные данные
4 4
1 3
1 2 1
2 3 2
3 4 5
4 1 4
выходные данные
3

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
    static constexpr long long INF = 1e16;

    struct Item {
        unsigned node;
        long long dist;

        friend bool operator<(const Item &a, const Item &b) {
            return a.dist < b.dist || (a.dist == b.dist && a.node < b.node);
        }
    };

    struct NodeState {
        std::vector<Item> children;
        long long dist;

        NodeState() :
            children{},
            dist{INF} {}

        // I guess rule of 5 is actually implicitly implemented for this
    };

    std::vector<NodeState> graph;

    GraphState() :
        graph{} {}

    GraphState(unsigned nodeCnt) :
        graph(nodeCnt) {}

    void dijkstra(unsigned start) {
        graph[start].dist = 0;
        std::set<Item> queue;

        queue.insert({start, 0});

        while (!queue.empty()) {
            Item cur = *queue.begin();
            queue.erase(queue.begin());

            if (graph[cur.node].dist < cur.dist)
                continue;

            for (const Item &next : graph[cur.node].children) {
                if (cur.dist + next.dist >= graph[next.node].dist)
                    continue;

                queue.insert({next.node, graph[next.node].dist = cur.dist + next.dist});
            }
        }
    }

    inline void link(unsigned from, unsigned to, long long weight) {
        graph[from].children.push_back({to, weight});
        graph[to].children.push_back({from, weight});
    }
};


int main() {
    unsigned n = 0,
             m = 0,
             s = 0,
             t = 0;
    int res = scanf("%u %u %u %u", &n, &m, &s, &t);
    assert(res == 4);

    GraphState gs{n};

    for (unsigned i = 0; i < m; ++i) {
        unsigned from = 0,
                 to = 0;
        long long weight = 0;

        res = scanf("%u %u %lld", &from, &to, &weight);
        assert(res == 3);

        gs.link(from - 1, to - 1, weight);
    }

    gs.dijkstra(s - 1);

    printf("%lld\n", gs.graph[t - 1].dist == gs.INF ? -1ll : gs.graph[t - 1].dist);

    return 0;
}


/*

Dijkstra

*/

