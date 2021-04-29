/*

������ �������� ��������� � �������� �������������� ������. ��� ��������� ������� � ����������� ����� ���� ��� ��������� �� h ������.

� ������� ���� ����-����: �� �������, ��� ������ ������ ������ ������� �������. ��� ��������� �������������� ����������� ������������ ��������� ������:

��������� �� a ������ �����
��������� �� b ������ �����
��������� �� c ������ �����
��������� �� ������ ����
������� ���� ��������� �� ������ �����. �������� ���������� ��� ������ ������, ����� ������� �� ������ ����. ���� �������� �������� ������������� �� ����, �������� �� ����������, �� ���� ������ ���� �� ������ ��� ������ �� ����� �� ������� ������� �������, ���� �� ������������.

����� ��������, ��� ���� ������� ����������, ������ ����� ���������� ���������� ������, �� ������� �������� ������� � ��� �������.

������� ������
� ������ ������ �������� ����� ����� h � ���������� ������ ��������� (1\N{LESS-THAN OR EQUAL TO}h\N{LESS-THAN OR EQUAL TO}1018).

�� ������ ������ �������� ����� ����� a, b � c � ��������� ����� (1\N{LESS-THAN OR EQUAL TO}a,b,c\N{LESS-THAN OR EQUAL TO}100000).

�������� ������
�������� ���� ����� ����� � ���������� ������, ��������� � ������� � ������� �����.

������
������� ������
15
4 7 9
�������� ������
9

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
    static constexpr unsigned long long INF = (unsigned long long)(1e18) + 100;

    struct Item {
        unsigned node;
        unsigned long long dist;

        friend bool operator<(const Item &a, const Item &b) {
            return a.dist < b.dist || (a.dist == b.dist && a.node < b.node);
        }
    };

    struct NodeState {
        std::vector<Item> children;
        unsigned long long dist;

        NodeState() :
            children{},
            dist{INF} {}

        // I guess rule of 5 is actually implicitly implemented for this
    };

    std::vector<NodeState> graph;

    GraphState() :
        graph{} {}

    GraphState(unsigned nodeCnt) :
        graph(nodeCnt) {}

    void dijkstra(unsigned start, unsigned long long limit) {
        graph[start].dist = 0;
        std::set<Item> queue;

        queue.insert({start, 0});

        while (!queue.empty()) {
            Item cur = *queue.begin();
            queue.erase(queue.begin());

            if (graph[cur.node].dist < cur.dist || graph[cur.node].dist >= limit || cur.dist >= limit)
                continue;

            for (const Item &next : graph[cur.node].children) {
                if (cur.dist + next.dist >= graph[next.node].dist)
                    continue;

                queue.insert({next.node, graph[next.node].dist = cur.dist + next.dist});
            }
        }
    }

    inline void link(unsigned from, unsigned to, unsigned long long weight) {
        graph[from].children.push_back({to, weight});
    }
};


int main() {
    unsigned long long h = 0;
    unsigned a = 0,
             b = 0,
             c = 0;
    int res = scanf("%llu %u %u %u", &h, &a, &b, &c);
    assert(res == 4);

    if (c < a) std::swap(c, a);
    if (c < b) std::swap(c, b);

    GraphState gs{c};

    for (unsigned i = 0; i < c; ++i) {
        gs.link(i, (i + a) % c, a);
        gs.link(i, (i + b) % c, b);
    }

    gs.dijkstra(0, h);

    unsigned long long reachable = 0;

    for (unsigned i = 0; i < c; ++i) {
        /*if (gs.graph[i].dist >= h)
            continue;*/

        reachable += std::max(0ll, (long long)(h / c - gs.graph[i].dist / c + (gs.graph[i].dist % c < h % c)));
    }

    printf("%llu\n", reachable);

    return 0;
}


/*

We build a graph of floors by modulo max(a, b, c) (let it be c), and then create edges, corresponding to transitions +a and +b.
Then Dijkstra on it tells us the actual number of floors we have to travel, to get to this floor modulo c, and so
we can calculate the number of times we will be able to reach this floor with different /c values

*/
