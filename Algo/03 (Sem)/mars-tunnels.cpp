#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>
#include <set>
#include <algorithm>


struct Dsu;

struct GraphState {
    struct NodeState {
        std::vector<unsigned> children;
        unsigned tin, tout;
        bool used;

        NodeState() :
            children{},
            tin{(unsigned)-1},
            tout{(unsigned)-1},
            used{false} {}

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
            used = true;
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

    bool dfs(unsigned from, unsigned type, Dsu &components);

    inline void link(unsigned from, unsigned to) {
        graph[from].children.push_back(to);
    }
};


struct Dsu {
    static constexpr unsigned BAD_IND = (unsigned)-1;

    struct DsuNode {
        unsigned parent;
        unsigned size;

        DsuNode() :
            parent{BAD_IND},
            size{0} {}

        DsuNode(unsigned new_parent) :
            parent{new_parent},
            size{1} {}

        inline bool isRoot() const {
            return !parent;
        }
    };

    std::vector<DsuNode> buf;

    Dsu() :
        buf{} {}

    Dsu(unsigned size) :
        buf(size) {}

    unsigned find(unsigned src) {
        std::vector<unsigned> path{};

        while (src != BAD_IND) {
            path.push_back(src);
            src = buf[src].parent;
        }

        src = path.back();
        path.pop_back();

        //unsigned prevSize = 0;

        for (unsigned cur : path) {
            buf[cur].parent = src;
            //buf[cur].size -= prevSize;
            //prevSize += buf[cur].size;
        }

        return src;
    }

    bool merge(unsigned a, unsigned b) {
        a = find(a);
        b = find(b);

        if (a == b)
            return false;

        //if (buf[a].size < buf[b].size)
        //    std::swap(a, b);

        buf[b].parent = a;
        //buf[a].size += buf[b].size;

        return true;
    }
};


bool GraphState::dfs(unsigned from, unsigned type, Dsu &components) {
    NodeState &curNode = graph[from];

    if (curNode.used)
        return false;

    curNode.enter(curTime);
    components.merge(from, type);

    for (unsigned to : curNode.children) {
        dfs(to, type, components);
    }

    curNode.exit(curTime);

    return false;
}


struct Edge {
    unsigned from;
    unsigned to;

    friend bool operator<(const Edge &a, const Edge &b) {
        return a.from < b.from || (a.from == b.from && a.to < b.to);
    }

    friend bool operator==(const Edge &a, const Edge &b) {
        return a.from == b.from && a.to == b.to;
    }
};


int main() {
    unsigned v = 0, e = 0, m = 0;
    int res = scanf("%u %u %u", &v, &e, &m);
    assert(res == 3);

    GraphState gs(v);

    std::set<Edge> toAdd;
    std::vector<Edge> removed;

    Dsu components{e};

    for (unsigned i = 0; i < e; ++i) {
        unsigned a = 0, b = 0;
        res = scanf("%u %u", &a, &b);
        assert(res == 2);

        if (a > b)
            std::swap(a, b);

        toAdd.insert(Edge{a, b});
    }

    for (unsigned i = 0; i < m; ++i) {
        unsigned a = 0, b = 0;
        res = scanf("%u %u", &a, &b);
        assert(res == 2);

        if (a > b)
            std::swap(a, b);

        removed.push_back(Edge{a, b});

        toAdd.erase(Edge{a, b});
    }

    for (const Edge &edge : toAdd) {
        gs.link(edge.from, edge.to);
        gs.link(edge.to, edge.from);
    }

    unsigned comp = 0;

    for (unsigned i = 0; i < v; ++i) {
        if (!gs.graph[i].used) {
            gs.dfs(i, i, components);
            comp++;
        }
    }

    std::vector<unsigned> ans(m + 1);
    unsigned idx = m;

    ans[idx--] = comp;

    std::reverse(removed.begin(), removed.end());

    for (const Edge &edge : removed) {
        comp -= components.merge(edge.from, edge.to);

        ans[idx--] = comp;
    }

    for (unsigned i : ans) {
        printf("%u\n", i);
    }

    return 0;
}
