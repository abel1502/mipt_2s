/*

Шреку необходимо пробраться в башню, чтобы спасти прекрасную принцессу Фиону. И сейчас его путь лежит через старый полуразвалившийся мост. Осёл очень боится упасть с этого моста в лавовую реку под ним и отказывается идти дальше, пока мост не будет полностью починен. Шрек не может идти без друга и решил отремонтировать его.

Мост представляет собой поле из n\N{MULTIPLICATION SIGN}m клеток, причем некоторые клетки ещё остались целыми. У Шрека есть только дощечки размера 1\N{MULTIPLICATION SIGN}2 (которые можно поворачивать и получать 2\N{MULTIPLICATION SIGN}1), установка которых занимает у него a секунд, и 1\N{MULTIPLICATION SIGN}1, установка которых занимает b секунд. Ему необходимо закрыть все пустые клетки, причем только их, не накладывая дощечки друг на друга.

Определите, через какое минимальное количество секунд Шрек и Осёл смогут продолжить свой путь дальше.

Входные данные
Первая строка входного файла содержит 4 целых числа n, m, a, b (1\N{LESS-THAN OR EQUAL TO}n,m\N{LESS-THAN OR EQUAL TO}100, |a|\N{LESS-THAN OR EQUAL TO}1000, |b|\N{LESS-THAN OR EQUAL TO}1000). Каждая из последующих n строк содержит по m символов: символ  «.» (точка) обозначает целую клетку моста, а символ «*» (звёздочка) — пустую.

Выходные данные
В выходной файл выведите одно число — минимальное количество секунд, через которое наши герои смогут продолжить свой путь, закрыв пустые клетки моста (их и только их).

Пример
входные данные
2 3 3 2
.**
.*.
выходные данные
5

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
        std::vector<unsigned> children;
        bool used       = false;
        unsigned match  = -1u;
    };

    std::vector<NodeState> graph;

    GraphState() :
        graph{} {}

    GraphState(unsigned nodeCnt) :
        graph(nodeCnt) {}

    bool augment(unsigned from) {
        if (graph[from].used)
            return false;

        graph[from].used = true;

        for (unsigned to : graph[from].children) {
            if (graph[to].match == -1u || augment(graph[to].match)) {
                graph[to].match = from;

                return true;
            }
        }

        return false;
    }

    void koon(unsigned width) {
        for (unsigned i = 0; i < graph.size(); ++i) {
            if ((i % width + i / width) & 1 || !augment(i))
                continue;

            for (unsigned j = 0; j < graph.size(); ++j) {
                graph[j].used = false;
            }
        }
    }

    inline void link(unsigned from, unsigned to) {
        graph[from].children.push_back(to);
    }
};


int main() {
    unsigned n = 0,
             m = 0;
    int      a = 0,
             b = 0;
    int res = scanf("%u %u %d %d\n", &n, &m, &a, &b);
    assert(res == 4);

    std::vector<std::vector<bool>> bridge(n, std::vector<bool>(m));
    unsigned missing = 0;

    for (unsigned i = 0; i < n; ++i) {
        char cur = 0;

        for (unsigned j = 0; j < m; ++j) {
            res = scanf("%c", &cur);
            assert(res == 1);

            bridge[i][j] = cur == '.';
            missing += cur == '*';
        }

        scanf("%c", &cur);
    }

    if (a >= 2 * b) {
        printf("%d\n", missing * b);
        return 0;
    }

    GraphState gs{n * m};

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j = 0; j < m; ++j) {
            if (bridge[i][j])
                continue;

            unsigned idx = i * m + j;

            #define NEIGHBOUR_(NI, NJ) {                                                \
                if ((NI) < n && (NJ) < m && !bridge[NI][NJ])                            \
                    gs.link(idx, (NI) * m + (NJ));                                      \
            }

            NEIGHBOUR_(i + 1, j    );
            NEIGHBOUR_(i - 1, j    );
            NEIGHBOUR_(i,     j + 1);
            NEIGHBOUR_(i,     j - 1);

            #undef NEIGHBOUR_
        }
    }

    gs.koon(m);

    unsigned ans = 0;
    for (GraphState::NodeState &cur : gs.graph) {
        ans += cur.match != -1u;
    }

    printf("%d\n", missing * b - ans * (2 * b - a));

    return 0;
}


/*

Koon this time (2 by 1 planks are edges in the polar graph of cells, split by the parity of the diagonals)

*/


