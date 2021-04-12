#include <cstdio>
#include <cstdarg>
#include <cerrno>

#include "general.h"


int verbosity = 0;

void err_(const char *funcName, int lineNo, const char *msg, ...) {
    va_list args = {};
    va_start(args, msg);

    if (::verbosity >= 1) {
        fprintf(stderr, "[ERROR in %s() on #%d] ", funcName, lineNo);
        vfprintf(stderr, msg, args);
        fprintf(stderr, "\n");
        if (errno != 0) {
            perror("System error:");
        }
    }

    va_end(args);
}

unsigned long long randLL() {
    static unsigned long long rnd = 123;

    rnd ^= rnd << 21;
    rnd ^= rnd >> 35;
    rnd ^= rnd << 4;

    return rnd;
}


