/*

Дан взвешенный ориентированный граф и вершина s в нем. Требуется для каждой вершины u найти длину кратчайшего пути из s в u.

Входные данные
Первая строка входного файла содержит n, m и s — количество вершин, ребер и номер выделенной вершины соответственно (2\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}2000, 1\N{LESS-THAN OR EQUAL TO}m\N{LESS-THAN OR EQUAL TO}6000).

Следующие m строк содержат описание ребер. Каждое ребро задается стартовой вершиной, конечной вершиной и весом ребра. Вес каждого ребра — целое число, не превосходящее 1015 по модулю. В графе могут быть кратные ребра и петли.

Выходные данные
Выведите n строк — для каждой вершины u выведите длину кратчайшего пути из s в u, '*' если не существует путь из s в u и '-' если не существует кратчайший путь из s в u.

Пример
входные данные
6 7 1
1 2 10
2 3 5
1 3 100
3 5 7
5 4 10
4 3 -18
6 1 -1
выходные данные
0
10
-
-
-
*

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
    static constexpr long long INF = 1e16;

    struct NodeState {
        std::vector<unsigned> children;
        long long dist;
        bool used;
        bool negLoop;

        NodeState() :
            children{},
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
        long long weight;

        Edge() :
            from{-1u},
            to{-1u},
            weight{INF} {}

        Edge(unsigned new_from, unsigned new_to, long long new_weight) :
            from{new_from},
            to{new_to},
            weight{new_weight} {}
    };

    std::vector<NodeState> graph;
    std::vector<Edge> edges;

    GraphState() :
        graph{} {}

    GraphState(unsigned nodeCnt) :
        graph(nodeCnt) {}

    void dfs(unsigned from) {
        NodeState &curNode = graph[from];

        if (curNode.used)
            return;

        curNode.enter();

        for (unsigned to : curNode.children) {
            dfs(to);
        }

        curNode.exit();

        return;
    }

    inline void link(unsigned from, unsigned to, long long weight) {
        graph[from].children.push_back(to);
        edges.push_back(Edge{from, to, weight});
    }

    void fordBellman(unsigned start) {
        graph[start].dist = 0;

        for (unsigned i = 0; i < graph.size() - 1; ++i)
            for (const Edge &edge : edges)
                if (graph[edge.from].dist != INF)
                    graph[edge.to].dist = std::min(graph[edge.to].dist, graph[edge.from].dist + edge.weight);

        for (const Edge &edge : edges) {
            if (graph[edge.from].dist == INF)
                continue;

            long long newDist = graph[edge.from].dist + edge.weight;

            if (newDist < graph[edge.to].dist) {
                //graph[edge.to].dist = newDist;
                graph[edge.to].negLoop = true;
                dfs(edge.to);
            }
        }
    }
};


int main() {
    unsigned n = 0,
             m = 0,
             s = 0;
    int res = scanf("%u %u %u", &n, &m, &s);
    assert(res == 3);

    GraphState gs{n};

    for (unsigned i = 0; i < m; ++i) {
        unsigned from = 0,
                 to = 0;
        long long weight = 0;

        res = scanf("%u %u %lld", &from, &to, &weight);
        assert(res == 3);

        gs.link(from - 1, to - 1, weight);
    }

    gs.fordBellman(s - 1);

    for (unsigned i = 0; i < n; ++i) {
        if (gs.graph[i].negLoop) {
            gs.dfs(i);
        }
    }

    for (unsigned i = 0; i < n; ++i) {
        if (gs.graph[i].dist == gs.INF)
            printf("*\n");
        else if (gs.graph[i].used)
            printf("-\n");
        else
            printf("%lld\n", gs.graph[i].dist);
    }

    return 0;
}


/*

Ford-Bellman, and then we mark everything reachable via infinite loops

*/

