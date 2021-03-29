/*

�������� ���������, ������� ��� ���� ������ ������ ����������, �������� �� ���� �� ��� ������� ������.

������� ������
������ ������ �������� ����� �������� ����������� ����� n (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}100000) � ���������� ������ � ������.

�� ������ ������ ��������� n �����. ��� ���� i-� ����� ������ ������ ���������� ����������������� �������� ������� � ������� i. ���� ����� �������� ����� ����, �� ������� �������� ������ ������.

� ������� ������ ��������� ����� m (1\N{LESS-THAN OR EQUAL TO}m\N{LESS-THAN OR EQUAL TO}100000)  � ���������� ��������. ������ �� ��������� m ����� �������� ��� ��������� ����� a � b (1\N{LESS-THAN OR EQUAL TO}a,b\N{LESS-THAN OR EQUAL TO}n).

�������� ������
��� ������� �� m �������� �������� �� ��������� ������ ����� 1, ���� ������� a �������� ����� �� ������� ������� b, � 0 � ��������� ������.

������
������� ������
6
0 1 1 2 3 3
5
4 1
1 4
3 6
2 6
6 5
�������� ������
0
1
1
0
0

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
        bool used;

        NodeState() :
            children{},
            tin{(unsigned)-1},
            tout{(unsigned)-1},
            used{false} {}

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

        inline bool enter(unsigned &curTime) {
            if (used) return true;

            tin = curTime++;
            used = true;

            return false;
        }

        inline void exit(unsigned &curTime) {
            assert(used);
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

    void dfs(unsigned from) {
        NodeState &curNode = graph[from];

        if (curNode.enter(curTime))
            return;

        for (unsigned to : curNode.children) {
            dfs(to);
        }

        curNode.exit(curTime);
    }

    inline void link(unsigned from, unsigned to) {
        graph[from].children.push_back(to);
    }
};


int main() {
    unsigned n = 0, m = 0;
    int res = scanf("%u", &n);
    assert(res == 1);

    GraphState gs(n);

    unsigned start = (unsigned)-1;

    for (unsigned i = 0; i < n; ++i) {
        unsigned parent = 0;
        res = scanf("%u", &parent);
        assert(res == 1);

        if (parent > 0) {
            gs.link(parent - 1, i);
        } else {
            start = i;
        }
    }

    assert(start != (unsigned)-1);

    gs.dfs(start);

    res = scanf("%u", &m);
    assert(res == 1);

    for (unsigned i = 0; i < m; ++i) {
        unsigned a = 0, b = 0;
        res = scanf("%u %u", &a, &b);
        assert(res == 2);

        GraphState::NodeState &nodeA = gs.graph[a - 1],
                              &nodeB = gs.graph[b - 1];

        printf("%u\n", (nodeA.tin < nodeB.tin && nodeB.tout < nodeA.tout));
    }

    return 0;
}


/*

Look, I've made a fancy (low-level) object-oriented graph.

*/
