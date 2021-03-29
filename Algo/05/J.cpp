/*

� ������ ������������� ������� ������ N ������. ����� ������������ �� ��� ���������� �����������. ����� ��������� �������, ����� � ����� ������� ����� ���� ������� �� ����� ������, ������ ������ ����� ���� ����������. ������ �������� ���, ��� ��������� �������������� ������ � ���� �������. ������������ ���������� ���������� ������������ ���������� �� ���� ����� ������� � ������� A. � ���������, �� �� ����� �������������� �� ��� ������������ �������, �� ������ ������������ �����. � ������� ���� ������� � ����� ����������� ����� ����� ��������� ����������� ��������� ���������. ���� ������ � ����� ����� ������� ��� �����������, ����� ��� ����������� ������ ���� ���������, � �� ���� �� ��������� ������. � ����� ������ �������� ���������� ������ ��������� �� ����� ������ ����, � ������� �� ����� ��������.

������� ������
� ������ ������ ��������� ����� N\N{LESS-THAN OR EQUAL TO}1000 � ����� A\N{LESS-THAN OR EQUAL TO}N. N ��������� ����� �������� �� N �����: � i-� ������ j-� ����� ����� 1, ���� ���� ����� �� ������� i � ������� j, ����� 0. ��������, ��� ������������� ������� ����� ������������� ���� ����������� � ������������ ���������� �� ����� ��� 32000 ����� �������.

�������� ������
�������� ������������������, � ������� ������� ������������ ������. ������ ������ ������ ��������� ��� ����� �����: ������ ������, � ����� � �� ����� ������� ��������� ��������� ����������. ����������� ������� ����������� � ������� �� ���������. �������������, ��� ������� ����������.

������
������� ������
4 2
0 0 1 0
0 0 1 0
1 1 0 1
0 0 1 0
�������� ������
2 1
1 4
4 1
1 2
2 4
4 2

*/


#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>
#include <stack>
#include <algorithm>


struct GraphState {
    std::vector<std::vector<bool>> graph;

    GraphState() :
        graph{} {}

    GraphState(unsigned nodeCnt) :
        graph(nodeCnt, std::vector<bool>(nodeCnt)) {}

    GraphState(const GraphState &other) :
        graph(other.graph) {}

    GraphState &operator=(const GraphState &other) {
        graph = other.graph;

        return *this;
    }

    void findEuler(unsigned from, std::vector<unsigned> &ans) {
        std::stack<unsigned> stack;
        stack.push(from);

        while (!stack.empty()) {
            unsigned node = stack.top();

            bool foundNext = false;

            for (unsigned i = 0; i < graph.size(); ++i) {
                if (graph[node][i]) {
                    graph[node][i] = false;
                    stack.push(i);

                    foundNext = true;

                    break;
                }
            }

            if (!foundNext) {
                ans.push_back(node);

                stack.pop();
            }
        }
    }
};


int main() {
    unsigned n = 0, start = 0;
    int res = scanf("%u %u", &n, &start);
    assert(res == 2);
    start -= 1;

    GraphState gs(n);

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j = 0; j < n; ++j) {
            unsigned state = 0;
            res = scanf("%u", &state);
            assert(res == 1);

            gs.graph[i][j] = !state && (i != j);
        }
    }

    std::vector<unsigned> ans;

    gs.findEuler(start, ans);

    std::reverse(ans.begin(), ans.end());

    if (ans.size() <= 1)
        return 0;

    printf("%u ", start + 1);

    for (unsigned i = 1; i < ans.size() - 1; ++i) {
        printf("%u\n%u ", ans[i] + 1, ans[i] + 1);
    }

    printf("%u\n", start + 1);

    return 0;
}


/*

Surprisingly simple: we just walk over the edges and voila

*/

