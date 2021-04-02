/*

Есть n лампочек и m переключателей. Каждый переключатель контролирует некоторое множество лампочек, а вот каждая лампочка контролируется ровно двумя переключателями. Если изменить положение переключателя, то изменят своё состояние все контролируемые им лампочки: горящие потухнут, а негорящие зажгутся.

Определите, можно ли так нажать на некоторые (возможно, никакие) переключатели, чтобы все лампочки зажглись.

Входные данные
Первая строка содержит два целых числа n и m (2\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}105, 2\N{LESS-THAN OR EQUAL TO}m\N{LESS-THAN OR EQUAL TO}105) — число лампочек и число переключателей.

Следующая строка содержит n целых чисел r1,r2,…,rn (0\N{LESS-THAN OR EQUAL TO}ri\N{LESS-THAN OR EQUAL TO}1) — изначальные состояния лапочек. Лампочка i включена, если и только если ri=1.

В каждой из следующих m строк содержится целое число x (0\N{LESS-THAN OR EQUAL TO}x\N{LESS-THAN OR EQUAL TO}n), а затем x различных целых чисел — количество лампочек, контролируемых очередным переключателем, а затем номера этих лампочек. Гарантируется, что каждая лампочка контролируется ровно двумя переключателями.

Выходные данные
Выведите "YES" без кавычек, если возможно включить все лампочки одновременно, иначе выведите "NO" без кавычек.

Примеры
входные данные
3 3
1 0 1
2 1 3
2 1 2
2 2 3
выходные данные
NO
входные данные
3 3
1 0 1
3 1 2 3
1 2
2 1 3
выходные данные
YES
входные данные
3 3
1 0 1
3 1 2 3
2 1 2
1 3
выходные данные
NO

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


struct LampInfo {
    static const unsigned BAD_IND = (unsigned)-1;

    bool initial;
    unsigned switches[2];

    LampInfo() :
        initial(false),
        switches{BAD_IND, BAD_IND} {}

    LampInfo(bool new_initial, unsigned switchA, unsigned switchB) :
        initial(new_initial),
        switches{switchA, switchB} {}

    LampInfo(const LampInfo &other) = default;

    LampInfo &operator=(const LampInfo &other) = default;

    void addSwitch(unsigned ind) {
        assert(switches[1] == BAD_IND);

        if (switches[0] == BAD_IND)
            switches[0] = ind;
        else
            switches[1] = ind;
    }

    void encode(GraphState &gs) {
        unsigned s00 = switches[0] << 1;
        unsigned s01 = s00 | 1;
        unsigned s10 = switches[1] << 1;
        unsigned s11 = s10 | 1;

        //printf(">> %u %u : %u %u %u %u\n", switches[0], switches[1], s00, s01, s10, s11);

        if (initial) {
            gs.link(s00, s10);
            gs.link(s01, s11);
            gs.link(s10, s00);
            gs.link(s11, s01);
        } else {
            gs.link(s00, s11);
            gs.link(s01, s10);
            gs.link(s10, s01);
            gs.link(s11, s00);
        }
    }
};


int main() {
    unsigned n = 0, m = 0;
    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    GraphState gs(m << 1);

    std::vector<LampInfo> lamps(n);

    for (unsigned i = 0; i < n; ++i) {
        unsigned r = 0;
        res = scanf("%u", &r);
        assert(res == 1);

        lamps[i].initial = r;
    }

    for (unsigned i = 0; i < m; ++i) {
        unsigned x = 0;
        res = scanf("%u", &x);
        assert(res == 1);

        for (unsigned j = 0; j < x; ++j) {
            unsigned lamp = 0;
            res = scanf("%u", &lamp);
            assert(res == 1);

            lamps[lamp - 1].addSwitch(i);
        }
    }

    for (unsigned i = 0; i < n; ++i) {
        lamps[i].encode(gs);
    }

    unsigned cat = 1;

    for (unsigned i = 0; i < m << 1; ++i) {
        if (!gs.graph[i].type) {
            //printf(">> %u -> %u\n", i + 1, cat);
            gs.dfs(i, cat++);
        }

    }

    /*for (unsigned i = 0; i < m << 1; ++i) {
        const GraphState::NodeState &node = gs.graph[i];

        printf("> %u (%u) -> ", i >> 1, i & 1);

        for (unsigned child : node.children) {
            printf("%u ", child);
        }

        printf(": %u\n", node.type);
    }*/

    for (unsigned i = 0; i < m; ++i) {
        if (gs.graph[i << 1].type == gs.graph[(i << 1) | 1].type) {
            printf("NO\n");
            return 0;
        }
    }

    printf("YES\n");

    return 0;
}


/*

2-SAT

*/

