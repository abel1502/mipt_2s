/*

��� ����� ����������������� ���� � N ��������� � M �������. � ����� ����������� ����� � ������� �����.

���������� ���������� ��������� ��������� �����.

������� ������
���� ����� �� ������� ����� ��������� �������: ������ ������ �������� ����� N � M (1\N{LESS-THAN OR EQUAL TO}N\N{LESS-THAN OR EQUAL TO}20000,1\N{LESS-THAN OR EQUAL TO}M\N{LESS-THAN OR EQUAL TO}200000). ������ �� ��������� M ����� �������� �������� ����� � ��� ����� ����� �� ��������� �� 1 �� N � ������ ������ �����.

�������� ������
�� ������ ������ ��������� ����� �������� ����� L � ���������� ��������� ��������� ��������� �����. �� ��������� ������ �������� N ����� �� ��������� �� 1 �� L � ������ ��������� ���������, ������� ����������� ��������������� �������. ���������� ��������� ������� ������������ �� 1 �� L ������������ �������.

������
������� ������
4 2
1 2
3 4
�������� ������
2
1 1 2 2

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

    for (unsigned i = 0; i < m; ++i) {
        unsigned a = 0, b = 0;
        res = scanf("%u %u", &a, &b);
        assert(res == 2);

        gs.link(a - 1, b - 1);
        gs.link(b - 1, a - 1);
    }

    unsigned comp = 1;

    for (unsigned i = 0; i < n; ++i) {
        if (!gs.graph[i].type) {
            gs.dfs(i, comp++);
        }
    }

    printf("%u\n", comp - 1);

    for (unsigned i = 0; i < n; ++i) {
        printf("%u ", gs.graph[i].type);
    }

    printf("\n");

    return 0;
}


/*

A simpler version of E

*/
