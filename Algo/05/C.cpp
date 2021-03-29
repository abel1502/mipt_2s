/*

��� ��������������� ������������ ���� ��� ������ � ������� ����. ���������� ���������� ���� �� � �� �����, � ���� ����, �� ������� ����� �� ���.

������� ������
� ������ ������ �������� ����� ��������� ��� ����������� ����� N � M (1\N{LESS-THAN OR SLANTED EQUAL TO}N\N{LESS-THAN OR SLANTED EQUAL TO}100000, M\N{LESS-THAN OR SLANTED EQUAL TO}100000) � ���������� ������ � ���� � ����� ��������������. ����� � M ������� ����������� ���� �����. ������ ����� ������� ����� �����  � �������� ��������� � �������� ������ ��������������.

�������� ������
���� � ����� ��� �����, �� ������� �NO�, �����  � �YES� � ����� ����������� ��� ������� � ������� ������ �����.

�������
������� ������
2 2
1 2
2 1
�������� ������
YES
1 2
������� ������
3 3
1 2
2 3
1 3
�������� ������
NO

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
            N_OLD,
            N_LOOP
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

    bool dfs(unsigned from, std::vector<unsigned> &loop) {
        NodeState &curNode = graph[from];

        if (curNode.used == NodeState::N_OLD)
            return false;

        curNode.enter(curTime);

        for (unsigned to : curNode.children) {
            if (graph[to].used == NodeState::N_CUR) {
                graph[to].used = NodeState::N_LOOP;

                loop.push_back(from);

                return true;
            }

            if (dfs(to, loop)) {
                loop.push_back(from);

                if (curNode.used == NodeState::N_LOOP) {
                    loop.push_back((unsigned)-1);
                }

                return true;
            }
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
    }

    std::vector<unsigned> ans;

    for (unsigned i = 0; i < n; ++i) {
        if (gs.dfs(i, ans)) {
            printf("YES\n");

            unsigned j = 0;

            for (; j < ans.size() && ans[j] != (unsigned)-1; ++j) {};
            assert(j < ans.size() && ans[j] == (unsigned)-1);
            --j;

            for (; j != (unsigned)-1; --j) {
                printf("%u ", ans[j] + 1);
            }

            printf("\n");

            return 0;
        }
    }

    printf("NO\n");

    return 0;
}


/*

If dfs hits a gray node, it's a loop

*/
