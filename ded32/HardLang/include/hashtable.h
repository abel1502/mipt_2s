#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "general.h"
#include "filebuf.h"
#include "checksum.h"

#include <cstring>
#include <cassert>
#include <algorithm>
#include <cctype>


namespace abel {

/// Warning: T WILL be copied a lot, so either make it simple enough or store it by pointers
template <typename T>
class Hashtable {
public:
    #undef R_OK

    enum result_e {
        R_OK = 0,
        R_BADMEMORY,
        R_BADPTR,
        R_BADSIZE,
        R_NOTFOUND,
        R_BADFMT,
        R_NOTIMPL,    /// A very special result, saying the feature you're trying to use should be pesent, but isn't yet implemented
        // TODO
    };

    static constexpr unsigned DEFAULT_CAPACITY = 256;
    static constexpr unsigned KEY_LEN = 64;  // TODO: Maybe 56 to have the struct's total size be a power of 2
    static constexpr unsigned CRITICAL_LOAD_FACTOR = 0x80;  // per 0x100
    static constexpr unsigned RESIZE_FACTOR = 2;

    using key_t = char[KEY_LEN];
    //typedef char key_t[KEY_LEN];

    enum NodeType_e {
        NODE_FULL = 0,
        NODE_FREE,
        NODE_DELETED,
    };

    struct Node {
        key_t key;
        T value;
        NodeType_e type;
        // TODO: Maybe hold a precomputed crc32 here for fast rehashing

        Node() :
            key{""},
            value{},
            type{NODE_FREE} {}
    };

    FACTORIES(Hashtable)

    result_e ctor() {
        return ctor(DEFAULT_CAPACITY);
    }

    result_e ctor(unsigned new_capacity) {
        assert(new_capacity > 0);

        capacity = new_capacity;
        size = 0;
        deleted = 0;

        buf = (Node *)calloc(capacity, sizeof(Node));

        if (!buf) {
            return lastResult = R_BADMEMORY;
        }

        for (unsigned i = 0; i < capacity; ++i) {
            buf[i].type = NODE_FREE;
        }

        return lastResult = R_OK;
    }

    result_e ctor(FileBuf *src) {
        if (!src)
            return lastResult = R_BADPTR;

        if (ctor((unsigned)src->getLineCnt() + 2))  // TODO: Maybe change size for testing purposes
            return lastResult;

        char *rawBuf = src->getData();
        char cur = 0;

        while (true) {
            key_t curKey = "";

            unsigned i = 0;

            for (; i < KEY_LEN - 1 && (cur = *(rawBuf++)) && cur != '='; ++i) {
                curKey[i] = cur;
            }

            memset(curKey + i, 0, KEY_LEN - i);

            if (!cur)
                break;
            else if (cur != '=')
                return lastResult = R_BADFMT;

            rawBuf++;

            while (isspace(cur = *(rawBuf++))) {}

            set(curKey, rawBuf - 2);

            while (cur && cur != '\n') {
                cur = *(rawBuf++);
            }

            if (!cur)
                break;

            *(rawBuf - 1) = '\0';
        }

        return lastResult = R_OK;
    }

    void dtor() {
        if (buf)
            free(buf);

        buf = nullptr;
    }

    void swap(Hashtable &other) noexcept {
        std::swap(lastResult, other.lastResult);
        std::swap(capacity, other.capacity);
        std::swap(size, other.size);
        std::swap(buf, other.buf);
    }

    Hashtable() = default;
    Hashtable(const Hashtable &other) = delete;
    Hashtable(const Hashtable &&other) = delete;
    Hashtable &operator=(const Hashtable &other) = delete;
    Hashtable &operator=(const Hashtable &&other) = delete;

    inline unsigned getCapacity() const {
        return capacity;
    }

    inline unsigned getSize() const {
        return size;
    }

    inline unsigned getDeleted() const {
        return deleted;
    }

    inline result_e getLastResult() const {
        return lastResult;
    }

    result_e validate() const{
        if (lastResult)
            return lastResult;

        if (size + deleted > capacity)
            return lastResult = R_BADSIZE;

        if (!buf)
            return lastResult = R_BADPTR;

        return lastResult = R_OK;
    }

    /// @param [out] key_t
    static void castToKey(const char *src, key_t key){
        memset(key, 0, KEY_LEN);
        strncpy(key, src, KEY_LEN - 1);

        assert(strlen(key) < KEY_LEN);
    }

    static void castToKey(const char *src, key_t key, unsigned len){
        assert(len < KEY_LEN);

        memset(key, 0, KEY_LEN);
        strncpy(key, src, len);

        assert(strlen(key) < KEY_LEN);
    }

    #ifdef __GNUC__

    static inline bool keycmp_asm(const key_t key1, const key_t key2) {
        unsigned res1 = false, res2 = false;

        asm (
            "vmovdqu ymm7, [%[key1]]\n"
            "vmovdqu ymm8, [%[key2]]\n"
            "vpcmpeqq ymm7, ymm7, ymm8\n"
            "vpmovmskb %[res1], ymm7\n"
            "vmovdqu ymm7, [%[key1]+32]\n"
            "vmovdqu ymm8, [%[key2]+32]\n"
            "vpcmpeqq ymm7, ymm7, ymm8\n"
            "vpmovmskb %[res2], ymm7\n"
            "not %[res1]\n"
            "not %[res2]\n"
            : [res1]"=&r"(res1), [res2]"=r"(res2)
            : [key1]"r"(key1), [key2]"r"(key2)
            : "cc", "ymm7", "ymm8"
        );

        return res1 | res2;
    }

    #ifdef NO_OPTIMIZE
    #define KEYCMP(KEY1, KEY2)  memcmp(KEY1, KEY2, KEY_LEN)
    #else
    #define KEYCMP(KEY1, KEY2)  keycmp_asm(KEY1, KEY2)
    #endif

    #else

    #define KEYCMP(KEY1, KEY2)  memcmp(KEY1, KEY2, KEY_LEN)

    #endif // __GNUC__

    //--------------------------------------------------------------------------------

    bool has(const char *key, unsigned len) const {
        key_t paddedKey = "";
        castToKey(key, paddedKey, len);

        return has_(paddedKey);
    }

    result_e set(const char *key, unsigned len, const T &value) {
        key_t paddedKey = "";
        castToKey(key, paddedKey, len);

        return set_(paddedKey, value);
    }

    result_e get(const char *key, unsigned len, T *value) const {
        key_t paddedKey = "";
        castToKey(key, paddedKey, len);

        return get_(paddedKey, value);
    }

    T get(const char *key, unsigned len) const {
        key_t paddedKey = "";
        castToKey(key, paddedKey, len);

        return get_(paddedKey);
    }

    result_e del(const char *key, unsigned len) {
        key_t paddedKey = "";
        castToKey(key, paddedKey, len);

        return del_(paddedKey);
    }

    result_e pop(const char *key, unsigned len, T *value) {
        key_t paddedKey = "";
        castToKey(key, paddedKey, len);

        return pop_(paddedKey, value);
    }

    T pop(const char *key, unsigned len) {
        key_t paddedKey = "";
        castToKey(key, paddedKey, len);

        return pop_(paddedKey);
    }

    bool has(const char *key) const {
        key_t paddedKey = "";
        castToKey(key, paddedKey);

        return has_(paddedKey);
    }

    result_e set(const char *key, const T &value) {
        key_t paddedKey = "";
        castToKey(key, paddedKey);

        return set_(paddedKey, value);
    }

    result_e get(const char *key, T *value) const {
        key_t paddedKey = "";
        castToKey(key, paddedKey);

        return get_(paddedKey, value);
    }

    T get(const char *key) const {
        key_t paddedKey = "";
        castToKey(key, paddedKey);

        return get_(paddedKey);
    }

    result_e del(const char *key) {
        key_t paddedKey = "";
        castToKey(key, paddedKey);

        return del_(paddedKey);
    }

    result_e pop(const char *key, T *value) {
        key_t paddedKey = "";
        castToKey(key, paddedKey);

        return pop_(paddedKey, value);
    }

    T pop(const char *key) {
        key_t paddedKey = "";
        castToKey(key, paddedKey);

        return pop_(paddedKey);
    }

    //--------------------------------------------------------------------------------

    result_e maybeFlushDeletions() {
        if (deleted << 2 < capacity)  // TODO: Maybe tweak this or export it as constants
            return lastResult = R_OK;

        return resize(capacity);
    }

    result_e resize(unsigned new_capacity) {
        if (new_capacity < capacity)
            return lastResult = R_BADSIZE;

        Hashtable new_ht;
        if (new_ht.ctor(new_capacity)) {
            return lastResult = new_ht.lastResult;
        }

        for (unsigned i = 0; i < capacity; ++i) {
            if (buf[i].type != NODE_FULL)
                continue;

            if (new_ht.set(buf[i].key, buf[i].value)) {
                lastResult = new_ht.lastResult;
                new_ht.dtor();
                return lastResult;
            }
        }

        swap(new_ht);
        new_ht.dtor();

        return lastResult = R_OK;
    }

    #define DUMP_(...)  fprintf(dfile, __VA_ARGS__)

    void dump() const {
        FILE *dfile = fopen("dump.txt", "wb");

        if (!dfile) {
            assert(false);
            return;
        }

        DUMP_("Hashtable (status: %u)\n"
              "capacity: %u\n"
              "size: %u\n"
              "deleted: %u\n", lastResult, capacity, size, deleted);

        unsigned freeStart = 0;
        bool isFree = false;

        for (unsigned i = 0; i < capacity; ++i) {
            if (buf[i].type == NODE_FREE) {
                if (!isFree) {
                    isFree = true;
                    freeStart = i;
                }

                continue;
            }

            if (isFree) {
                if (i == freeStart + 1)
                    DUMP_("%u: FREE\n", freeStart);
                else
                    DUMP_("%u-%u: FREE\n", freeStart, i - 1);

                isFree = false;
            }

            if (buf[i].type == NODE_DELETED) {
                DUMP_("%u: DELETED (%u)\n", i, hash(buf[i].key));
            } else {
                DUMP_("%u: \"%s\" (%u) -> \"%p\"\n", i, buf[i].key, hash(buf[i].key), buf[i].value);
            }
        }

        fclose(dfile);
    }

    #undef DUMP_

    const Node *debugGetBuf() const {
        return buf;
    }

private:
    mutable result_e lastResult;

    unsigned capacity;
    unsigned size;
    unsigned deleted;

    Node *buf;

    inline unsigned hash(const key_t key) const {
        //return crc32_compute(key, KEY_LEN) % capacity;  // On average 1140 items per bucket
        //return (0x61C88647ULL * (unsigned long long)crc32_compute(key, KEY_LEN) >> 32) % capacity;  // Exactly the same, indicating it was a problem with crc itself. I guess it shouldn't be used in hashtables
        // return fnv1a_64(key, KEY_LEN) % (unsigned long long)capacity;  // 1.5 per bucket. One. Point. Five. I don't think any intrinsicness of crc32 can compensate for this. (Although I'll still test it)
        // return fnv1a_64_asm(key, KEY_LEN) % (unsigned long long)capacity;
        #ifdef NO_OPTIMIZE
        return fnv1a_64(key, KEY_LEN) % (unsigned long long)capacity;
        #else
        return crc32_asm(key, KEY_LEN) % (unsigned long long)capacity;
        #endif

        // TODO: If I stick to fnv1a_64, I could as well remove the key length limitation due to redundancy. It would probably only speed things up.
    }

    //--------------------------------------------------------------------------------

    bool has_(const key_t key) const {
        T result{};

        return get_(key, &result) == R_OK;
    }

    result_e set_(const key_t key, const T &value) {
        // TODO: resize if necessary
        if (size << 8 >= capacity * CRITICAL_LOAD_FACTOR) {  // TODO: Maybe swap for size << 1 >= capacity
            resize(capacity * RESIZE_FACTOR);
        }

        unsigned curInd = hash(key);
        unsigned endInd = (curInd - 1 + capacity) % capacity;

        Node *deletedSpot = nullptr;

        assert(curInd < capacity);

        for (;; curInd = (curInd + 1) % capacity) {
            // TODO: Assigning to deleted nodes may be wrong... ?
            // hash(a) == hash(b) == 0
            // hash(c) == 1
            // [ ][ ][ ][ ][ ][ ][ ]
            // [a][ ][ ][ ][ ][ ][ ]
            // [a][c][ ][ ][ ][ ][ ]
            // [a][c][b][ ][ ][ ][ ]
            // [a][#][b][ ][ ][ ][ ]
            // [a][b][b][ ][ ][ ][ ]

            Node *curNode = &buf[curInd];

            switch (curNode->type) {
            case NODE_FULL:
                break;

            case NODE_FREE:
                memcpy(curNode->key, key, KEY_LEN);
                curNode->value = value;
                curNode->type = NODE_FULL;

                return lastResult = R_OK;

            case NODE_DELETED:
                deletedSpot = deletedSpot ? deletedSpot : curNode;
                continue;

            default:
                ERR("Shouldn't be reachable");
                abort();
            }

            if (KEYCMP(key, curNode->key) == 0) {
                curNode->value = value;
                curNode->type = NODE_FULL;

                return lastResult = R_OK;
            }

            if (curInd == endInd)
                break;
        }

        if (deletedSpot) {
            memcpy(deletedSpot->key, key, KEY_LEN);
            deletedSpot->value = value;
            deletedSpot->type = NODE_FULL;

            return lastResult = R_OK;
        }

        // TODO: Shouldn't be reachable

        return lastResult = R_BADSIZE;
    }

    result_e get_(const key_t key, T *value) const {
        T result = get_(key);

        if (lastResult) {
            return lastResult;
        }

        *value = result;
        return lastResult = R_OK;
    }

    T get_(const key_t key) const {
        unsigned curInd = hash(key);
        unsigned endInd = (curInd - 1 + capacity) % capacity;

        assert(curInd < capacity);

        for (;; curInd = (curInd + 1) % capacity) {
            Node *curNode = &buf[curInd];

            switch (curNode->type) {
            case NODE_FULL:
                break;

            case NODE_FREE:
                lastResult = R_NOTFOUND;
                return {};

            case NODE_DELETED:
                continue;

            default:
                ERR("Shouldn't be reachable");
                abort();
            }

            if (KEYCMP(key, curNode->key) == 0) {
                lastResult = R_OK;
                return curNode->value;
            }

            if (curInd == endInd)
                break;
        }

        lastResult = R_NOTFOUND;
        return {};
    }

    result_e del_(const key_t key) {
        unsigned curInd = hash(key);
        unsigned endInd = (curInd - 1 + capacity) % capacity;

        assert(curInd < capacity);

        for (;; curInd = (curInd + 1) % capacity) {
            Node *curNode = &buf[curInd];

            switch (curNode->type) {
            case NODE_FULL:
                break;

            case NODE_FREE:
                return lastResult = R_OK;  // TODO: R_NOTFOUND?

            case NODE_DELETED:
                continue;

            default:
                ERR("Shouldn't be reachable");
                abort();
            }

            if (KEYCMP(key, curNode->key) == 0) {
                curNode->value = nullptr;
                curNode->type = NODE_DELETED;

                return lastResult = R_OK;
            }

            if (curInd == endInd)
                break;
        }

        return lastResult = R_OK;  // TODO: Same as above
    }

    result_e pop_(const key_t key, T *value) {
        if (get_(key, value))
            return lastResult;

        return del_(key);
    }

    T pop_(const key_t key) {
        T result = get_(key);
        if (lastResult)
            return nullptr;

        del_(key);
        if (lastResult)
            return nullptr;

        return result;
    }
};

}

#endif
