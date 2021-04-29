/*

Ребята собираются на экскурсию в Кострому. Кострома — красивый старый город, в котором площади соединяются друг с другом короткими улицами, каждую из которых можно пройти не более чем за десять минут. Ребята хотят составить интересный маршрут экскурсии. Так как они поедут на автобусах, то маршрут экскурсии должен начинаться и оканчиваться на одной и той же площади. К сожалению, у ребят будет очень мало времени. Поэтому они решили выбрать наиболее короткий кольцевой маршрут, не проходящий ни по какой улице дважды.

Помогите ребятам найти такой маршрут.

Входные данные
Первая строка входного файла содержит два натуральных числа n и m — количество площадей и улиц в Костроме (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}1000, 1\N{LESS-THAN OR EQUAL TO}m\N{LESS-THAN OR EQUAL TO}10000). Площади занумерованы от 1 до n.

Последующие m строк содержат описания улиц. Каждая улица описывается тремя целыми числами — номерами площадей, которые она соединяет, и количеством минут, которые требуются ребятам на то, чтобы пройти по ней (от одной до десяти минут). Между двумя площадями может быть более одной улицы. Улица соединяет две различные площади.

Гарантируется, что в Костроме существует как минимум один кольцевой маршрут.

Выходные данные
Первая строка выходного файла должна содержать единственное число — продолжительность минимального маршрута в минутах.

Пример
входные данные
5 6
1 2 1
2 3 10
1 3 1
2 4 1
3 4 1
1 5 1
выходные данные
4

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
    static constexpr long long INF = 1e16;

    struct EdgeState {
        unsigned to;
        long long weight;
        unsigned used;  // That's also a piece of advice from my friend - this allows to avoid resetting it every iteration
        unsigned twin;
    };

    unsigned iteration;
    std::vector<long long> dists;
    std::vector<std::vector<EdgeState>> edges;

    GraphState() :
        iteration{0},
        dists{},
        edges{} {}

    GraphState(unsigned nodeCnt) :
        iteration{0},
        dists(nodeCnt, INF),
        edges(nodeCnt) {}

    long long dijkstra(unsigned start) {
        constexpr unsigned K = 11;

        iteration++;

        dists.assign(dists.size(), INF);
        dists[start] = 0;

        std::vector<std::queue<unsigned>> queues(K);
        queues[0].push(start);

        unsigned pos = 0,
                 cnt = 1;

        long long minLoop = INF;

        while (cnt) {
            while (queues[pos].empty())
                pos = (pos + 1) % K;

            unsigned cur = queues[pos].front();
            queues[pos].pop();
            cnt--;

            /*if (used[cur])
                continue;

            used[cur] = true;*/

            for (EdgeState &next : edges[cur])
                if (next.used != iteration)
                    minLoop = std::min(minLoop, dists[cur] + dists[next.to] + next.weight);

            for (EdgeState &next : edges[cur]) {
                if (dists[cur] + next.weight >= dists[next.to])
                    continue;

                next.used = iteration;
                edges[next.to][next.twin].used = iteration;

                dists[next.to] = dists[cur] + next.weight;
                queues[dists[next.to] % K].push(next.to);
                cnt++;
            }
        }

        return minLoop;
    }

    inline void link(unsigned from, unsigned to, long long weight) {
        edges[to].push_back({from, weight, 0, (unsigned)edges[from].size()});
        edges[from].push_back({to, weight, 0, (unsigned)edges[to].size() - 1});
    }
};


int main() {
    unsigned n = 0,
             m = 0;
    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    GraphState gs{n};

    for (unsigned i = 0; i < m; ++i) {
        unsigned from = 0,
                 to = 0;
        long long weight = 0;

        res = scanf("%u %u %lld", &from, &to, &weight);
        assert(res == 3);

        gs.link(from - 1, to - 1, weight);
    }

    long long minLoop = gs.INF;
    for (unsigned s = 0; s < n; ++s)
        minLoop = std::min(minLoop, gs.dijkstra(s));

    printf("%lld\n", minLoop);

    return 0;
}


/*

~As a friend hinted to me, apparently running Dijkstra for every vertex works. But we have to be quick, so I might switch to dirtier code~

Screw him, now I have to remake it in 1-10 bfs

*/


