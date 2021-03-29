/*

Дан ориентированный невзвешенный граф. Необходимо его топологически отсортировать.

Входные данные
В первой строке входного файла даны два натуральных числа N и M (1\N{LESS-THAN OR EQUAL TO}N\N{LESS-THAN OR EQUAL TO}100000,M\N{LESS-THAN OR EQUAL TO}100000) — количество вершин и рёбер в графе соответственно. Далее в M строках перечислены рёбра графа. Каждое ребро задаётся парой чисел — номерами начальной и конечной вершин соответственно.

Выходные данные
Вывести любую топологическую сортировку графа в виде последовательности номеров вершин. Если граф невозможно топологически отсортировать, вывести -1.

Пример
входные данные
6 6
1 2
3 2
4 2
2 5
6 5
4 6
выходные данные
4 6 3 1 2 5

*/

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>
#include <algorithm>


struct GraphState {
    struct NodeState {
        std::vector<unsigned> children;
        unsigned tin, tout;
        enum {
            N_NEW,
            N_CUR,
            N_OLD
        } used;

        NodeState() :
            children{},
            tin{(unsigned)-1},
            tout{(unsigned)-1},
            used{N_NEW} {}

        NodeState(const NodeState &other) :
            children(other.children),
            tin{other.tin},
            tout{other.tout},
            used{other.used} {}

        NodeState &operator=(const NodeState &other) {
            children = other.children;
            tin = other.tin;
            tout = other.tout;
            used = other.used;

            return *this;
        }

        inline void enter(unsigned &curTime) {
            tin = curTime++;
            used = N_CUR;
        }

        inline void exit(unsigned &curTime) {
            //assert(used == N_CUR);
            tout = curTime++;
            used = N_OLD;
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

    bool dfs(unsigned from, std::vector<unsigned> &sorted) {
        NodeState &curNode = graph[from];

        if (curNode.used == NodeState::N_OLD)
            return false;

        curNode.enter(curTime);

        for (unsigned to : curNode.children) {
            if (graph[to].used == NodeState::N_CUR || dfs(to, sorted)) {
                return true;
            }
        }

        curNode.exit(curTime);

        sorted.push_back(from);

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
    }

    std::vector<unsigned> sorted;

    for (unsigned i = 0; i < n; ++i) {
        if (gs.dfs(i, sorted)) {
            printf("-1\n");

            return 0;
        }
    }

    reverse(sorted.begin(), sorted.end());

    for (unsigned i = 0; i < sorted.size(); ++i) {
        printf("%u ", sorted[i] + 1);
    }

    printf("\n");

    return 0;
}


/*

Standard dfs topsort

*/

