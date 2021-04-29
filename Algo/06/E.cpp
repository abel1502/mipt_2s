/*

Рик решил на день почувствовать себя бизнесменом!

В городе есть несколько обменников валюты. В рамках данной задачи считаем, что каждый обменник специализируется только на двух валютах и может производить операции только с ними. Возможно, существуют обменники, специализирующиеся на одинаковых парах валют. В каждом обменнике — свой обменный курс: курс обмена A на B — это количество единиц валюты B, выдаваемое за 1 единицу валюты A. Также в каждом обменнике есть комиссия — сумма, которую вы должны заплатить, чтобы производить операцию. Комиссия взимается в той валюте, которую меняет клиент.

Например, если вы хотите поменять 100 долларов США на русские рубли в обменнике, где курс обмена равен 29.75, а комиссия равна 0.39, вы получите (100\N{MINUS SIGN}0.39)\N{DOT OPERATOR}29.75=2963.3975 рублей (эх, были времена).

Вы точно знаете, что в городе используется всего N валют. Пронумеруем их числами 1,2,…,N. Тогда каждый обменник представляют 6 чисел: целые A и B — номера обмениваемых валют, а также вещественные RAB,CAB,RBA и CBA — обменные курсы и комиссии при переводе из A в B и из B в A соответственно.

Рик обладает некоторой суммой в валюте S. Он задаётся вопросом, можно ли, после нескольких операций обмена увеличить свой капитал. Конечно, он хочет, чтобы в конце его деньги вновь были в валюте S. Помогите ему ответить на его вопрос. Рик должен всегда должен иметь неотрицательную сумму денег.

Входные данные
Первая строка содержит четыре числа: N — количество валют, M — количество обменников, S — валюта, которой располагает Рик, и V — количество единиц этой валюты. Следующие M строк содержат по 6 чисел, которые задают описание соответствующих обменников в описанном выше формате. Числа разделяются одним или несколькими пробелами. Выполняются ограничения 1\N{LESS-THAN OR SLANTED EQUAL TO}S\N{LESS-THAN OR SLANTED EQUAL TO}N\N{LESS-THAN OR SLANTED EQUAL TO}100,1\N{LESS-THAN OR SLANTED EQUAL TO}M\N{LESS-THAN OR SLANTED EQUAL TO}100, V — вещественное число, 0\N{LESS-THAN OR SLANTED EQUAL TO}V\N{LESS-THAN OR SLANTED EQUAL TO}103.

В каждом обменнике курсы обмена и комиссии — вещественные числа, задаваемые с точностью не выше двух знаков после запятой в десятичном представлении, причём 10\N{MINUS SIGN}2\N{LESS-THAN OR SLANTED EQUAL TO}rate\N{LESS-THAN OR SLANTED EQUAL TO}102,0\N{LESS-THAN OR SLANTED EQUAL TO}comission\N{LESS-THAN OR SLANTED EQUAL TO}102.

Назовём последовательность операций обмена простой, если каждый обменник используется в ней не более одного раза. Вы можете считать, что отношение итоговой суммы Рика к исходной не будет превосходить 104 при любой простой последовательности операций.

Выходные данные
Если Рик может увеличить свой капитал, выведите YES, иначе выведите NO.

Пример
входные данные
3 2 1 10.0
1 2 1.0 1.0 1.0 1.0
2 3 1.1 1.0 1.1 1.0
выходные данные
NO

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


struct GraphState {
    static constexpr long double INF = 0.;

    struct NodeState {
        long double dist;
        bool used;
        bool negLoop;

        NodeState() :
            dist{INF},
            used{false},
            negLoop{false} {}

        // I guess rule of 5 is actually implicitly implemented for this

        inline void enter() {
            used = true;
        }

        inline void exit() {
        }
    };

    struct Edge {
        unsigned from;
        unsigned to;
        long double rate;
        long double fee;

        Edge() :
            from{-1u},
            to{-1u},
            rate{0.},
            fee{0.} {}

        Edge(unsigned new_from, unsigned new_to, long double new_rate, long double new_fee) :
            from{new_from},
            to{new_to},
            rate{new_rate},
            fee{new_fee} {}
    };

    std::vector<NodeState> graph;
    std::vector<Edge> edges;

    GraphState() :
        graph{} {}

    GraphState(unsigned nodeCnt) :
        graph(nodeCnt) {}

    inline void link(unsigned from, unsigned to, long double rate, long double fee) {
        edges.push_back(Edge{from, to, rate, fee});
    }

    bool fordBellman(unsigned start, long double initial) {
        graph[start].dist = initial;

        for (unsigned i = 0; i < graph.size() * 2; ++i)
            for (const Edge &edge : edges)
                if (graph[edge.from].dist != INF)
                    graph[edge.to].dist = std::max(graph[edge.to].dist, (graph[edge.from].dist - edge.fee) * edge.rate);

        for (unsigned i = 0; i < graph.size() - 1; ++i) {
            for (const Edge &edge : edges) {
                if (graph[edge.from].dist == INF)
                    continue;

                if ((graph[edge.from].dist - edge.fee) * edge.rate > graph[edge.to].dist) {
                    return true;
                }
            }
        }

        return graph[start].dist > initial;
    }
};


int main() {
    unsigned n = 0,
             m = 0,
             s = 0;
    long double v = 0.;
    int res = scanf("%u %u %u %Lg", &n, &m, &s, &v);
    assert(res == 4);

    GraphState gs{n};

    for (unsigned i = 0; i < m; ++i) {
        unsigned from = 0, to = 0;
        long double rate_ab = 0., rate_ba = 0., fee_ab = 0., fee_ba = 0.;

        res = scanf("%u %u %Lg %Lg %Lg %Lg", &from, &to, &rate_ab, &fee_ab, &rate_ba, &fee_ba);
        assert(res == 6);

        gs.link(from - 1, to - 1, rate_ab, fee_ab);
        gs.link(to - 1, from - 1, rate_ba, fee_ba);
    }

    if (gs.fordBellman(s - 1, v))
        printf("YES\n");
    else
        printf("NO\n");

    return 0;
}


/*

We just look for a positive loop from s with Ford-Bellman. Max told me regular doubles caused issues, so hello 80-bit floats)

*/


