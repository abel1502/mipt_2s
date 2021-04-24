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
#include <chrono>
#include <getopt.h>  // Has to be included after std headers, breaks them

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


static bool genGoodTestKeys(unsigned count, Hashtable::key_t *buf, const Hashtable *ht) {
    unsigned capacity = ht->getCapacity();
    const Hashtable::Node *htBuf = ht->debugGetBuf();

    if (ht->getSize())
        return true;

    for (unsigned i = 0; i < count; ++i) {
        unsigned idx = randLL() % capacity;

        while (htBuf[idx].value < Hashtable::NODE_SPECIAL) {
            idx = (idx + 1) % capacity;
        }

        memcpy(buf[i], htBuf[idx].key, Hashtable::KEY_LEN);
    }

    return false;
}


static bool genBadTestKeys(unsigned count, Hashtable::key_t *buf) {
    for (unsigned i = 0; i < count; ++i) {
        memset(buf, 0, Hashtable::KEY_LEN);

        unsigned len = randLL() % Hashtable::KEY_LEN;

        for (unsigned j = 0; j < len; ++j) {
            buf[i][j] = randLL() & 0xff;
        }
    }

    return false;
}


#ifdef __GNUC__
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE __declspec(noinline)
#endif

#define TEST_FUNC_(NAME)                                                                                                                            \
 NOINLINE void test##NAME(const Hashtable *ht, unsigned testCount, unsigned testKeyCount, const Hashtable::key_t *testKeys) {                       \
    printf("Testing " #NAME " keys...\n");                                                                                                          \
                                                                                                                                                    \
    std::chrono::high_resolution_clock clk{};                                                                                                       \
    auto tStart = clk.now();                                                                                                                        \
    volatile static char *result = nullptr;                                                                                                         \
    while (testCount--) {                                                                                                                           \
        result = ht->get(testKeys[randLL() % testKeyCount]);                                                                                        \
    }                                                                                                                                               \
                                                                                                                                                    \
    auto tEnd = clk.now();                                                                                                                          \
                                                                                                                                                    \
    printf("Done, on average %lfms per query\n", (double)(tEnd - tStart).count() / (double)testCount * 1000.);                                      \
}

TEST_FUNC_(Good)
TEST_FUNC_(Mixed)

#undef TEST_FUNC_


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
    REQUIRE(!ht.ctor(&tbuf));

    /*REQUIRE(!ht.ctor(32));

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
    REQUIRE(!ht.set(curKey, "success 3"));*/

    //ht.dump();

    constexpr unsigned TEST_KEY_CNT = 10000;
    
    Hashtable::key_t *testKeys = (Hashtable::key_t *)calloc(TEST_KEY_CNT * 2, sizeof(Hashtable::key_t));

    REQUIRE(!genGoodTestKeys(TEST_KEY_CNT, testKeys, &ht));
    REQUIRE(!genBadTestKeys(TEST_KEY_CNT, testKeys + TEST_KEY_CNT));

    /*const Hashtable::key_t TEST_GOOD_KEYS[3] = {
        "superexcellent",
        "hash table",
        "implementation",
    };

    const Hashtable::key_t TEST_BAD_KEYS[3] = {
        "Anyone who dislikes my profiling",
        "Ъ1 (although it's actually quite a good key)",
        "<Insert your password here>",
    };

    const Hashtable::key_t TEST_ANY_KEYS[6] = {
        "superexcellent",
        "hash table",
        "implementation",
        "Anyone who dislikes my profiling",
        "Ъ1 (although it's actually quite a good key)",
        "<Insert your password here>",
    };*/

    //ht.dump();

    /*srand(123);

    for (unsigned i = 0; i < TEST_CNT; ++i) {
        volatile char *result = ht.get(TEST_ANY_KEYS[rand() % 6]);
    }*/

    testGood(&ht, 100000, TEST_KEY_CNT, testKeys);
    testMixed(&ht, 5000, TEST_KEY_CNT * 2, testKeys);

    free(testKeys);

    const Hashtable::key_t TKEY = "блестяще";
    printf("Hashtable condition: %s\n", ht.get(TKEY));

    ht.dtor();

    printf("Done.\n\n");

    CLEANUP_;

    return 0;

    #undef CLEANUP_
}
