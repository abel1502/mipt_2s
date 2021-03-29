/*

Дан неориентированный граф. Требуется найти все мосты в нем.

Входные данные
Первая строка входного файла содержит два натуральных числа n и m — количество вершин и ребер графа соответственно (n\N{LESS-THAN OR EQUAL TO}20000,m\N{LESS-THAN OR EQUAL TO}200000).

Следующие m строк содержат описание ребер по одному на строке. Ребро номер i описывается двумя натуральными числами bi,ei — номерами концов ребра (1\N{LESS-THAN OR EQUAL TO}bi,ei\N{LESS-THAN OR EQUAL TO}n).

Выходные данные
Первая строка выходного файла должна содержать одно натуральное число k — количество мостов в заданном графе. На следующей строке выведите k целых чисел — номера ребер, которые являются мостами, в возрастающем порядке. Ребра нумеруются с единицы в том порядке, в котором они заданы во входном файле.

Пример
входные данные
6 7
1 2
2 3
3 4
1 3
4 5
4 6
5 6
выходные данные
1
3

*/


#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>
#include <set>
#include <algorithm>


struct Edge {
    unsigned from, to;

    Edge() :
        from{(unsigned)-1},
        to{(unsigned)-1} {}

    Edge(unsigned new_from, unsigned new_to) :
        from{std::min(new_from, new_to)},
        to{std::max(new_from, new_to)} {}

    friend bool operator<(const Edge &a, const Edge &b) {
        if (a.from != b.from)   return a.from < b.from;
        else                    return a.to   < b.to;
    }
};


struct GraphState {
    struct NodeState {
        std::vector<unsigned> children;
        unsigned tin, tout, ret;
        bool used;

        NodeState() :
            children{},
            tin{(unsigned)-1},
            tout{(unsigned)-1},
            ret{(unsigned)-1},
            used{false} {}

        NodeState(const NodeState &other) :
            children(other.children),
            tin{other.tin},
            tout{other.tout},
            ret{other.ret},
            used{other.used} {}

        NodeState &operator=(const NodeState &other) {
            children = other.children;
            tin = other.tin;
            tout = other.tout;
            ret = other.ret;
            used = other.used;

            return *this;
        }

        inline void enter(unsigned &curTime) {
            ret = tin = curTime++;
            used = true;
        }

        inline void exit(unsigned &curTime) {
            tout = curTime++;
        }
    };

    std::vector<NodeState> graph;
    unsigned curTime;

    GraphState() :
        graph{},
        curTime{0} {}

    GraphState(unsigned nodeCnt) :
        graph(nodeCnt),
        curTime{0} {}

    GraphState(const GraphState &other) :
        graph{other.graph},
        curTime{other.curTime} {}

    GraphState &operator=(const GraphState &other) {
        graph = other.graph;
        curTime = other.curTime;

        return *this;
    }

    bool dfs(unsigned from, std::set<Edge> &ans, unsigned parent = (unsigned)-1) {
        NodeState &curNode = graph[from];

        if (curNode.used)
            return false;

        curNode.enter(curTime);

        for (unsigned to : curNode.children) {
            if (to == parent)
                continue;

            if (!graph[to].used) {
                dfs(to, ans, from);

                if (graph[to].ret >= graph[to].tin) {
                    ans.insert({from, to});
                }
            }

            curNode.ret = std::min(curNode.ret, graph[to].ret);
        }

        curNode.exit(curTime);

        return false;
    }

    inline void link(unsigned from, unsigned to) {
        graph[from].children.push_back(to);
    }
};


int main() {
    unsigned n = 0, m = 0;
    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    GraphState gs(n);
    std::vector<Edge> edges;

    for (unsigned i = 0; i < m; ++i) {
        unsigned a = 0, b = 0;
        res = scanf("%u %u", &a, &b);
        assert(res == 2);

        gs.link(a - 1, b - 1);
        gs.link(b - 1, a - 1);

        edges.push_back({a - 1, b - 1});
    }

    std::set<Edge> ans;

    for (unsigned i = 0; i < n; ++i) {
        gs.dfs(i, ans);
    }

    printf("%zu\n", ans.size());

    for (unsigned i = 0; i < m; ++i) {
        if (ans.find(edges[i]) != ans.end()) {
            printf("%u ", i + 1);
        }
    }

    printf("\n");

    return 0;
}


/*

Whatever we were told at class (i.e. dfs with magic counters)

*/
