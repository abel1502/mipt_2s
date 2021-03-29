/*

Требуется найти количество ребер в конденсации ориентированного графа. Примечание: конденсация графа не содержит кратных ребер.

Входные данные
Первая строка входного файла содержит два натуральных числа n и m — количество вершин и рёбер графа соответственно (n\N{LESS-THAN OR EQUAL TO}10000, m\N{LESS-THAN OR EQUAL TO}100000). Следующие m строк содержат описание ребер, по одному на строке. Ребро номер i описывается двумя натуральными числами bi,ei — началом и концом ребра соответственно (1\N{LESS-THAN OR EQUAL TO}bi,ei\N{LESS-THAN OR EQUAL TO}n). В графе могут присутствовать кратные ребра и петли.

Выходные данные
Первая строка выходного файла должна содержать одно число — количество ребер в конденсации графа.

Пример
входные данные
4 4
2 1
3 2
2 3
4 3
выходные данные
2

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
        unsigned tin, tout;
        unsigned type;

        NodeState() :
            children{},
            tin{(unsigned)-1},
            tout{(unsigned)-1},
            type{0} {}

        NodeState(const NodeState &other) :
            children(other.children),
            tin{other.tin},
            tout{other.tout},
            type{other.type} {}

        NodeState &operator=(const NodeState &other) {
            children = other.children;
            tin = other.tin;
            tout = other.tout;
            type = other.type;

            return *this;
        }

        inline void enter(unsigned &curTime, unsigned new_type) {
            tin = curTime++;
            type = new_type;
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

    bool dfsTopsort(unsigned from, std::vector<unsigned> &sorted) {
        NodeState &curNode = graph[from];

        if (curNode.type)
            return false;

        curNode.enter(curTime, 1);

        for (unsigned to : curNode.children) {
            /*if (graph[to].type == 1 || dfsTopsort(to, sorted)) {
                return true;
            }*/

            dfsTopsort(to, sorted);
        }

        curNode.exit(curTime);

        sorted.push_back(from);

        return false;
    }

    bool dfs(unsigned from, unsigned type) {
        NodeState &curNode = graph[from];

        if (curNode.type)
            return false;

        curNode.enter(curTime, type);

        for (unsigned to : curNode.children) {
            dfs(to, type);
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
    GraphState gsr(n);

    for (unsigned i = 0; i < m; ++i) {
        unsigned a = 0, b = 0;
        res = scanf("%u %u", &a, &b);
        assert(res == 2);

        if (a == b || find(gs.graph[a - 1].children.begin(), gs.graph[a - 1].children.end(), b - 1) != gs.graph[a - 1].children.end())
            continue;

        gs.link(a - 1, b - 1);
        gsr.link(b - 1, a - 1);
    }

    std::vector<unsigned> sorted;

    for (unsigned i = 0; i < n; ++i) {
        if (gs.dfsTopsort(i, sorted)) {
            assert(false);
        }
    }

    reverse(sorted.begin(), sorted.end());

    for (unsigned i : sorted) {
        gsr.dfs(i, i + 1);
    }

    std::vector<std::set<unsigned>> seen(n);
    unsigned condEdgeCnt = 0;

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j : gs.graph[i].children) {
            unsigned catI = gsr.graph[i].type - 1;
            unsigned catJ = gsr.graph[j].type - 1;

            if (catI != catJ && seen[catI].find(catJ) == seen[catI].end()) {
                seen[catI].insert(catJ);
                condEdgeCnt++;
            }
        }
    }

    printf("%u\n", condEdgeCnt);

    return 0;
}


/*

Kosarayo (or whatever his name is)

*/

