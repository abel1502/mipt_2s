#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <set>
#include <queue>


#pragma GCC push_options
#pragma GCC optimize ("unroll-loops")

struct State {
    unsigned char pos[8];

    static const unsigned GOAL = (((((((((((((0 << 4) | 1) << 4) | 2) << 4) | 3) << 4) | 4) << 4) | 5) << 4) | 6) << 4) | 7;

    State() :
        pos{} {}

    State(unsigned encoded) {
        for (unsigned i = 0; i < 8; ++i) {
            pos[7 - i] = encoded & 0b1111;
            encoded >>= 4;
        }
    }

    unsigned encode() const {
        unsigned encoded = 0;

        for (unsigned i = 0; i < 8; ++i) {
            encoded = (encoded << 4) | pos[i];
        }

        return encoded;
    }

    unsigned children(unsigned results[4]) {
        unsigned spacePos = 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8;

        for (unsigned i = 0; i < 8; ++i) {
            spacePos -= pos[i];
        }

        unsigned spaceRow = spacePos % 3;
        unsigned spaceCol = spacePos / 3;

        unsigned curRes = 0;

        for (unsigned i = 0; i < 8; ++i) {
            if ((pos[i] % 3 == spaceRow && (pos[i] / 3 + 1 == spaceCol || pos[i] / 3 == spaceCol + 1)) ||
                (pos[i] / 3 == spaceCol && (pos[i] % 3 + 1 == spaceRow || pos[i] % 3 == spaceRow + 1))) {

                //printf(">> %u gives ", i + 1);

                char backupPos = pos[i];
                pos[i] = spacePos;

                assert(curRes < 4);

                results[curRes++] = encode();

                //printf("%u\n", encode());

                pos[i] = backupPos;
            }
        }

        return curRes;
    }

    void log() {
        char buf[9] = {' ' - 1, ' ' - 1, ' ' - 1, ' ' - 1, ' ' - 1, ' ' - 1, ' ' - 1, ' ' - 1, ' ' - 1};

        for (unsigned i = 0; i < 8; ++i) {
            buf[pos[i]] = '0' + i;
        }

        for (unsigned i = 0; i < 9; ++i) {
            printf("%c", buf[i] + 1);
            if (i % 3 == 2) {
                printf("\n");
            }
        }
    }
};

#pragma GCC pop_options


unsigned bfs(unsigned from) {
    unsigned children[4] = {};
    std::set<unsigned> used{};

    struct QueueItem {
        unsigned state;
        unsigned depth;
    };

    std::queue<QueueItem> queue{};
    queue.push({from, 0});

    while (!queue.empty()) {
        QueueItem curItem = queue.front();
        queue.pop();

        State cur(curItem.state);

        unsigned curEncoded = cur.encode();

        if (used.count(curEncoded) > 0) {
            continue;
        }

        //printf(">\n");
        //cur.log();

        used.insert(curEncoded);

        if (curEncoded == State::GOAL) {
            return curItem.depth;
        }

        unsigned curChildern = cur.children(children);

        for (unsigned i = 0; i < curChildern; ++i) {
            queue.push({children[i], curItem.depth + 1});
        }
    }

    return -2;
}


int main() {
    State initial{};

    for (unsigned i = 0; i < 9; ++i) {
        int cur = '\n';

        while (cur == '\r' || cur == '\n') {
            cur = fgetc(stdin);

            assert(cur != EOF);
        }

        if (cur == ' ') {
            continue;
        }

        assert(cur >= '1' && cur - '0' < 10);

        initial.pos[cur - '1'] = i;
    }

    unsigned inversions = 0;

    for (unsigned i = 0; i < 8; ++i) {
        for (unsigned j = i + 1; j < 8; ++j) {
            inversions += (initial.pos[i] > initial.pos[j]);
        }
    }

    if (inversions & 1) {
        printf("-1\n");
    } else {
        printf("%u\n", bfs(initial.encode()));
    }



    return 0;
}
