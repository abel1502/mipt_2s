/*

Требуется построить в двудольном графе минимальное контролирующее множество, если дано максимальное паросочетание.

Входные данные
В первой строке файла даны два числа m и n (1\N{LESS-THAN OR EQUAL TO}m,n\N{LESS-THAN OR EQUAL TO}4000) — размеры долей. Каждая из следующих m строк содержит список ребер, выходящих из соответствующей вершины первой доли. Этот список начинается с числа Ki (0\N{LESS-THAN OR EQUAL TO}Ki\N{LESS-THAN OR EQUAL TO}n) — количества ребер, после которого записаны вершины второй доли, соединенные с данной вершиной первой доли, в произвольном порядке. Сумма всех Ki во входном файле не превосходит 500000. Последняя строка файла содержит некоторое максимальное паросочетание в этом графе — m чисел 0\N{LESS-THAN OR EQUAL TO}Li\N{LESS-THAN OR EQUAL TO}n — соответствующая i-й вершине первой доли вершина второй доли, или 0, если i-я вершина первой доли не входит в паросочетание.

Выходные данные
Первая строка содержит размер минимального контролирующего множества. Вторая строка содержит количество вершин первой доли S, после которого записаны S чисел — номера вершин первой доли, входящих в контролирующее множество, в возрастающем порядке. Третья строка содержит описание вершин второй доли в аналогичном формате.

Пример
входные данные
3 2
2 1 2
1 2
1 2
1 2 0
выходные данные
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
