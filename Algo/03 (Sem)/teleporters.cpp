#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <vector>
#include <bitset>


int main() {
    // As Max pointed out, bitsets are amazingly convenient

    unsigned n = 0;
    int res = scanf("%u\n", &n);
    assert(res == 1);

    std::vector<std::bitset<2001>> dp(n);

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j = 0; j < n; ++j) {
            dp[i][j] = (fgetc(stdin) - '0');
        }

        fgetc(stdin);
    }

    for (unsigned k = 0; k < n; ++k) {
        for (unsigned i = 0; i < n; ++i) {
            if (dp[i][k])
                dp[i] |= dp[k];
        }
    }

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j = 0; j < n; ++j) {
            putc('0' + dp[i][j], stdout);
        }

        puts("");
    }

    return 0;
}
