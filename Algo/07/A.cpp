/*

Требуется найти в связном графе остовное дерево минимального веса.

Воспользуйтесь алгоритмом Прима.

Входные данные
Первая строка входного файла содержит два натуральных числа n и m — количество вершин и ребер графа соответственно. Следующие m строк содержат описание ребер по одному на строке. Ребро номер i описывается тремя натуральными числами bi, ei и wi — номера концов ребра и его вес соответственно (1\N{LESS-THAN OR EQUAL TO}bi,ei\N{LESS-THAN OR EQUAL TO}n, 0\N{LESS-THAN OR EQUAL TO}wi\N{LESS-THAN OR EQUAL TO}100000). n\N{LESS-THAN OR EQUAL TO}5000,m\N{LESS-THAN OR EQUAL TO}100000.
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


struct GraphState {
    static constexpr unsigned INF = 1000000000;

    struct Item {
        unsigned node;
        unsigned dist;

        friend bool operator<(const Item &a, const Item &b) {
            return a.dist < b.dist || (a.dist == b.dist && a.node < b.node);
        }
    };

    struct NodeState {
        std::vector<Item> children;
        unsigned dist = INF;
    };

    std::vector<NodeState> graph;

    GraphState() :
        graph{} {}

    GraphState(unsigned nodeCnt) :
        graph(nodeCnt) {}

    unsigned prim() {
        graph[0].dist = 0;
        std::set<Item> queue;

        for (unsigned i = 0; i < graph.size(); ++i) {
            queue.insert({i, graph[i].dist});
        }

        while (!queue.empty()) {
            Item cur = *queue.begin();
            queue.erase(queue.begin());

            for (const Item &next : graph[cur.node].children) {
                if (queue.find(Item{next.node, graph[next.node].dist}) == queue.end() || graph[next.node].dist <= next.dist)
                    continue;

                queue.erase(Item{next.node, graph[next.node].dist});
                queue.insert(Item{next.node, graph[next.node].dist = next.dist});
            }
        }

        unsigned ans = 0;
        for (unsigned i = 0; i < graph.size(); ++i) {
            ans += graph[i].dist;
        }

        return ans;
    }

    inline void link(unsigned from, unsigned to, unsigned weight) {
        graph[from].children.push_back({to, weight});
    }
};


int main() {
    unsigned n = 0,
             m = 0;
    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    GraphState gs{n};

    for (unsigned i = 0; i < m; ++i) {
        unsigned b = 0,
                 e = 0,
                 w = 0;

        res = scanf("%u %u %u", &b, &e, &w);
        assert(res == 3);

        gs.link(b - 1, e - 1, w);
        gs.link(e - 1, b - 1, w);
    }

    printf("%u\n", gs.prim());

    return 0;
}


/*

Prim, or whatever his name is.

*/
