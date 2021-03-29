/*

¬ состав √алактической империи входит N планет. ћежду большинством из них существуют гиперканалы. Ќовый император повелел, чтобы с любой планеты можно было попасть на любую другую, пройд€ только через один гиперканал.  аналы устроены так, что позвол€ют путешествовать только в одну сторону. ≈динственный оставшийс€ прокладчик гиперканалов расположен на базе около планеты с номером A.   сожалению, он не может путешествовать по уже существующим каналам, он всегда прокладывает новый. ј наличие двух каналов в одном направлении между двум€ планетами существенно осложн€ет навигацию. ¬аша задача Ц найти такой маршрут дл€ прокладчика, чтобы все необходимые каналы были построены, и не было бы построено лишних. ¬ конце своего маршрута прокладчик должен оказатьс€ на своей родной базе, с которой он начал движение.

¬ходные данные
¬ первой строке находитс€ число N\N{LESS-THAN OR EQUAL TO}1000 и число A\N{LESS-THAN OR EQUAL TO}N. N следующих строк содержит по N чисел: в i-й строке j-е число равно 1, если есть канал от планеты i к планете j, иначе 0. »звестно, что √алактическа€ импери€ может удовлетворить свою потребность в гиперканалах прокладкой не более чем 32000 новых каналов.

¬ыходные данные
¬ыведите последовательность, в которой следует прокладывать каналы.  ажда€ строка должна содержать два целых числа: номера планет, с какой и на какую следует проложить очередной гиперканал. √иперканалы следует перечислить в пор€дке их прокладки. √арантируетс€, что решение существует.

ѕример
входные данные
4 2
0 0 1 0
0 0 1 0
1 1 0 1
0 0 1 0
выходные данные
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

