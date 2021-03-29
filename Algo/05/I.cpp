/*

Дан неориентированный граф без петель и кратных рёбер. Требуется найти все точки сочленения в нем.

Входные данные
Первая строка входного файла содержит два натуральных числа n и m — количество вершин и ребер графа соответственно (n\N{LESS-THAN OR EQUAL TO}20000,m\N{LESS-THAN OR EQUAL TO}200000).

Следующие m строк содержат описание ребер по одному на строке. Ребро номер i описывается двумя натуральными числами bi,ei — номерами концов ребра (1\N{LESS-THAN OR EQUAL TO}bi,ei\N{LESS-THAN OR EQUAL TO}n).

Выходные данные
Первая строка выходного файла должна содержать одно натуральное число k — количество точек сочленения в заданном графе. На следующей строке выведите k целых чисел — номера вершин, которые являются точками сочленения, в возрастающем порядке.

Пример
входные данные
9 12
1 2
2 3
4 5
2 6
2 7
8 9
1 3
1 4
1 5
6 7
3 8
3 9
выходные данные
3
1 2 3

*/


#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>
#include <set>
#include <algorithm>


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

    bool dfs(unsigned from, std::set<unsigned> &ans, unsigned parent = (unsigned)-1) {
        NodeState &curNode = graph[from];

        if (curNode.used)
            return false;

        curNode.enter(curTime);

        unsigned distinctChildren = 0;

        for (unsigned to : curNode.children) {
            if (to == parent)
                continue;

            if (!graph[to].used) {
                dfs(to, ans, from);

                ++distinctChildren;

                if (parent != (unsigned)-1 && graph[to].ret >= curNode.tin) {
                    ans.insert(from);
                }

                curNode.ret = std::min(curNode.ret, graph[to].ret);
            } else {
                curNode.ret = std::min(curNode.ret, graph[to].tin);
            }
        }

        if (parent == (unsigned)-1 && distinctChildren > 1) {
            ans.insert(from);
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

    for (unsigned i = 0; i < m; ++i) {
        unsigned a = 0, b = 0;
        res = scanf("%u %u", &a, &b);
        assert(res == 2);

        gs.link(a - 1, b - 1);
        gs.link(b - 1, a - 1);
    }

    std::set<unsigned> ans;

    for (unsigned i = 0; i < n; ++i) {
        gs.dfs(i, ans);
    }

    printf("%zu\n", ans.size());

    for (unsigned i : ans) {
        printf("%u ", i + 1);
    }

    printf("\n");

    return 0;
}


/*

Very similar to H

*/
