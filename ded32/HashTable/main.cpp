/*

To define an unmangled alias {RAW} for the mangled names {MANGLED_GCC} (for gcc and clang) and {MANGLED_MSVC} (for msvc), do:

#if defined(__GNUC__)
#pragma weak {MANGLED_GCC}={RAW}
#elif defined(_MSC_VER)
#pragma comment(linker, "-alternatename:{MANGLED_MSVC}={RAW}"
#endif

*/

#undef UNICODE
#undef _UNICODE

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <getopt.h>

#define CHECKSUM_NOIMPL

#include "general.h"
#include "filebuf.h"
#include "hashtable.h"


using namespace abel;


static void showHelp(const char *binName) {
    printf("Usage:  %s [-h] [-v] -t tfile\n"
           "  -h        - show this and exit\n"
           "  -v        - increase verbosity\n"
           "  -t tfile  - run speed tests with tfile as the sample set\n"
           "\n", binName);
}


int main(int argc, char **argv) {
    FileBuf tbuf{};

    #define CLEANUP_ \
        tbuf.dtor();

    int c = 0;

    while ((c = getopt(argc, argv, "+vht:")) != -1) {
        switch (c) {
        case 'h':
            CLEANUP_;

            showHelp(argv[0]);
            return 0;

        case 't':
            if (tbuf.ctor(optarg, "r")) {
                CLEANUP_;

                ERR("Couldn't open %s to read", optarg);
                return 2;
            }
            break;

        case 'v':
            verbosity++;
            break;

        case '?':
            CLEANUP_;

            ERR("Unknown option: -%c.", optopt);
            showHelp(argv[0]);
            return 1;

        default:
            CLEANUP_;

            ERR("Shouldn't be reachable");
            abort();
        }
    }

    if (!tbuf.isInited()) {
        CLEANUP_;

        ERR("Both ifile and ofile are required");
        showHelp(argv[0]);
        return 1;
    }

    if (optind != argc) {
        CLEANUP_;

        ERR("Unexpected number of positional arguments.");
        showHelp(argv[0]);
        return 1;
    }

    Hashtable ht;
    REQUIRE(!ht.ctor(32));

    Hashtable::key_t curKey = "";

    ht.castToKey("test", curKey);
    REQUIRE(!ht.set(curKey, "success"));
    REQUIRE(!ht.set(curKey, "success 2"));
    ht.castToKey("debug", curKey);
    REQUIRE(!ht.set(curKey, "Huge success"));
    ht.castToKey("deleted", curKey);
    REQUIRE(!ht.set(curKey, "failure?"));
    REQUIRE(!ht.del(curKey));
    ht.castToKey("test", curKey);
    REQUIRE(!ht.set(curKey, "success 3"));

    ht.dump();

    ht.dtor();

    printf("Done.\n\n");

    CLEANUP_;

    return 0;

    #undef CLEANUP_
}
