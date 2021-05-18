/*

��������� ��������� � ���������� ����� ����������� �������������� ���������, ���� ���� ������������ �������������.

������� ������
� ������ ������ ����� ���� ��� ����� m � n (1\N{LESS-THAN OR EQUAL TO}m,n\N{LESS-THAN OR EQUAL TO}4000) � ������� �����. ������ �� ��������� m ����� �������� ������ �����, ��������� �� ��������������� ������� ������ ����. ���� ������ ���������� � ����� Ki (0\N{LESS-THAN OR EQUAL TO}Ki\N{LESS-THAN OR EQUAL TO}n) � ���������� �����, ����� �������� �������� ������� ������ ����, ����������� � ������ �������� ������ ����, � ������������ �������. ����� ���� Ki �� ������� ����� �� ����������� 500000. ��������� ������ ����� �������� ��������� ������������ ������������� � ���� ����� � m ����� 0\N{LESS-THAN OR EQUAL TO}Li\N{LESS-THAN OR EQUAL TO}n � ��������������� i-� ������� ������ ���� ������� ������ ����, ��� 0, ���� i-� ������� ������ ���� �� ������ � �������������.

�������� ������
������ ������ �������� ������ ������������ ��������������� ���������. ������ ������ �������� ���������� ������ ������ ���� S, ����� �������� �������� S ����� � ������ ������ ������ ����, �������� � �������������� ���������, � ������������ �������. ������ ������ �������� �������� ������ ������ ���� � ����������� �������.

������
������� ������
3 2
2 1 2
1 2
1 2
1 2 0
�������� ������
2
1 1
1 2

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

    struct NodeState {
        std::set<unsigned> children;
        bool used       = false;
        unsigned match  = -1u;
    };

    std::vector<NodeState> graph;

    GraphState() :
        graph{} {}

    GraphState(unsigned nodeCnt) :
        graph(nodeCnt) {}


    void dfs(unsigned from) {
        if (graph[from].used)
            return;

        graph[from].used = true;

        for (unsigned to : graph[from].children) {
            dfs(to);
        }
    }

    inline void link(unsigned from, unsigned to) {
        graph[from].children.insert(to);
    }

    inline void reverse(unsigned from, unsigned to) {
        graph[from].children.erase(to);
        graph[to].children.insert(from);
    }
};


int main() {
    unsigned n = 0,
             m = 0;
    int res = scanf("%u %u", &m, &n);
    assert(res == 2);

    GraphState gs{m + n};

    for (unsigned i = 0; i < m; ++i) {
        unsigned k = 0;
        res = scanf("%u", &k);
        assert(res == 1);

        for (unsigned j = 0; j < k; ++j) {
            unsigned cur = 0;
            res = scanf("%u", &cur);
            assert(res == 1);

            gs.link(i, m + cur - 1);
        }
    }

    for (unsigned i = 0; i < m; ++i) {
        unsigned cur = 0;
        res = scanf("%u", &cur);
        assert(res == 1);

        if (!cur)
            continue;

        cur = m + cur - 1;

        gs.reverse(i, cur);

        gs.graph[i].match = cur;
        gs.graph[cur].match = i;
    }

    for (unsigned i = 0; i < m; ++i) {
        if (gs.graph[i].match == -1u)
            gs.dfs(i);
    }

    std::vector<unsigned> l_neg{}, r_pos{};  // L- and R+

    for (unsigned i = 0; i < m + n; ++i) {
        if (i < m && !gs.graph[i].used)
            l_neg.push_back(i);

        if (i >= m && gs.graph[i].used)
            r_pos.push_back(i - m);
    }

    printf("%zu\n", l_neg.size() + r_pos.size());

    printf("%zu ", l_neg.size());
    for (unsigned i : l_neg)
        printf("%u ", i + 1);
    printf("\n");

    printf("%zu ", r_pos.size());
    for (unsigned i : r_pos)
        printf("%u ", i + 1);
    printf("\n");

    return 0;
}


/*

We do the L+/L-/R+/R- min vertex cover search, just as we were taught in the lections

*/
