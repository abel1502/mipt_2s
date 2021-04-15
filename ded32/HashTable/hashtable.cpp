#include "hashtable.h"

#include <cstring>
#include <cassert>
#include <algorithm>
#include <cctype>

//#include <intrin.h>


namespace abel {

#ifdef __GNUC__
const Hashtable::value_t Hashtable::NODE_FREE =    (Hashtable::value_t)0;
const Hashtable::value_t Hashtable::NODE_DELETED = (Hashtable::value_t)1;
const Hashtable::value_t Hashtable::NODE_SPECIAL = (Hashtable::value_t)256;
#endif

Hashtable::result_e Hashtable::ctor() {
    return ctor(DEFAULT_CAPACITY);
}

Hashtable::result_e Hashtable::ctor(unsigned new_capacity) {
    assert(new_capacity > 0);

    capacity = new_capacity;
    size = 0;
    deleted = 0;

    buf = (Node *)calloc(capacity, sizeof(Node));

    if (!buf) {
        return lastResult = R_BADMEMORY;
    }

    return lastResult = R_OK;
}

Hashtable::result_e Hashtable::ctor(FileBuf *src) {
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

void Hashtable::dtor() {
    if (buf)
        free(buf);

    buf = nullptr;
}

void Hashtable::swap(Hashtable &other) {
    std::swap(lastResult, other.lastResult);
    std::swap(capacity, other.capacity);
    std::swap(size, other.size);
    std::swap(buf, other.buf);
}

Hashtable::result_e Hashtable::validate() const {
    if (lastResult)
        return lastResult;

    if (size + deleted > capacity)
        return lastResult = R_BADSIZE;

    if (!buf)
        return lastResult = R_BADPTR;

    return lastResult = R_OK;
}

void Hashtable::castToKey(const char *src, key_t key) {
    memset(key, 0, KEY_LEN);
    strncpy(key, src, KEY_LEN - 1);
}

#ifdef __GNUC__

inline bool keycmp_asm(const Hashtable::key_t key1, const Hashtable::key_t key2) {
    unsigned res1 = false, res2 = false;

    asm(
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

#define KEYCMP(KEY1, KEY2)  keycmp_asm(KEY1, KEY2)

#else

#define KEYCMP(KEY1, KEY2)  memcmp(KEY1, KEY2, KEY_LEN)

#endif // __GNUC__

Hashtable::mvalue_t Hashtable::get(const key_t key) const {
    unsigned curInd = hash(key);
    unsigned endInd = (curInd - 1 + capacity) % capacity;

    assert(curInd < capacity);

    for (;; curInd = (curInd + 1) % capacity) {
        Node *curNode = &buf[curInd];

        if (curNode->value < NODE_SPECIAL) {
            if (curNode->value == NODE_FREE) {
                lastResult = R_NOTFOUND;
                return nullptr;
            }

            continue;
        }

        if (KEYCMP(key, curNode->key) == 0) {
            lastResult = R_OK;
            return const_cast<mvalue_t>(curNode->value);
        }

        if (curInd == endInd)
            break;
    }

    lastResult = R_NOTFOUND;
    return nullptr;
}

Hashtable::result_e Hashtable::get(const key_t key, mvalue_t *value) const {
    mvalue_t result = get(key);

    if (lastResult) {
        assert(!result);
        return lastResult;
    }

    *value = result;
    return lastResult = R_OK;
}

Hashtable::result_e Hashtable::set(const key_t key, value_t value) {
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

        if (curNode->value == NODE_DELETED) {
            deletedSpot = deletedSpot ? deletedSpot : curNode;
            continue;
        } else if (curNode->value == NODE_FREE) {
            memcpy(curNode->key, key, KEY_LEN);
            curNode->value = value;

            return lastResult = R_OK;
        }

        if (KEYCMP(key, curNode->key) == 0) {
            curNode->value = value;

            return lastResult = R_OK;
        }

        if (curInd == endInd)
            break;
    }

    if (deletedSpot) {
        memcpy(deletedSpot->key, key, KEY_LEN);
        deletedSpot->value = value;

        return lastResult = R_OK;
    }

    // TODO: Shouldn't be reachable

    return lastResult = R_BADSIZE;
}

Hashtable::result_e Hashtable::del(const key_t key) {
    unsigned curInd = hash(key);
    unsigned endInd = (curInd - 1 + capacity) % capacity;

    assert(curInd < capacity);

    for (;; curInd = (curInd + 1) % capacity) {
        Node *curNode = &buf[curInd];

        if (curNode->value < NODE_SPECIAL) {
            if (curNode->value == NODE_FREE) {
                return lastResult = R_OK;  // TODO: R_NOTFOUND?
            }

            continue;
        }

        if (KEYCMP(key, curNode->key) == 0) {
            curNode->value = NODE_DELETED;

            return lastResult = R_OK;
        }

        if (curInd == endInd)
            break;
    }

    return lastResult = R_OK;  // TODO: Same as above
}

Hashtable::result_e Hashtable::pop(const key_t key, mvalue_t *value) {
    if (get(key, value))
        return lastResult;

    return del(key);
}

Hashtable::mvalue_t Hashtable::pop(const key_t key) {
    mvalue_t result = get(key);
    if (lastResult)
        return nullptr;

    del(key);
    if (lastResult)
        return nullptr;

    return result;
}

Hashtable::result_e Hashtable::maybeFlushDeletions() {
    if (deleted << 2 < capacity)  // TODO: Maybe tweak this or export it as constants
        return lastResult = R_OK;

    return resize(capacity);
}

Hashtable::result_e Hashtable::resize(unsigned new_capacity) {
    if (new_capacity < capacity)
        return lastResult = R_BADSIZE;

    Hashtable new_ht;
    if (new_ht.ctor(new_capacity)) {
        return lastResult = new_ht.lastResult;
    }
    
    for (unsigned i = 0; i < capacity; ++i) {
        if (buf[i].value < NODE_SPECIAL)
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

/*void Hashtable::dump() const {
    FILE *dfile = fopen("dump", "wb");

    if (!dfile) {
        assert(false);
        return;
    }

    DUMP_("digraph Hashtable {\n"
          "graph [rankdir=LR splines=spline pack=true concentrate=true]\n");

    for (unsigned i = 0; i < capacity; ++i) {
        if (buf[i].value == NODE_FREE) {
            DUMP_("%u [fontname=Consolas shape=box style=filled color=black fillcolor=\"limegreen\" label=\"%u\\lFREE\"]\n", i, i);
        } else if (buf[i].value == NODE_DELETED) {
            DUMP_("%u [fontname=Consolas shape=box style=filled color=black fillcolor=\"crimson\" label=\"%u\\lDELETED (%u)\"]\n", i, i, hash(buf[i].key));
        } else {
            DUMP_("%u [fontname=Consolas shape=box style=filled color=black fillcolor=\"#0096FF\" label=<%u\\l\\\"%s\\\" (%u): \\\"%s\\\">]\n", i, i, buf[i].key, hash(buf[i].key), buf[i].value);
        }
                
        if (i + 1 < capacity)
            DUMP_("%u:e -> %u:w\n", i, i + 1);
    }

    DUMP_("}\n"
          "}\n");

    fclose(dfile);

    system("dot -O -Tsvg dump");
    system("start dump.svg");
}*/

void Hashtable::dump() const {
    FILE *dfile = fopen("dump.txt", "wb");

    if (!dfile) {
        assert(false);
        return;
    }

    /*DUMP_("Hashtable (status: %u)\n"
          "capacity: %u\n"
          "size: %u\n"
          "deleted: %u\n", lastResult, capacity, size, deleted);*/

    unsigned freeStart = 0;
    bool isFree = false;

    for (unsigned i = 0; i < capacity; ++i) {
        if (buf[i].value == NODE_FREE) {
            if (!isFree) {
                isFree = true;
                freeStart = i;
            }

            continue;
        }
        
        if (isFree) {
            /*if (i == freeStart + 1)
                DUMP_("%u: FREE\n", freeStart);
            else
                DUMP_("%u-%u: FREE\n", freeStart, i - 1);*/

            isFree = false;
        }

        if (buf[i].value == NODE_DELETED) {
            //DUMP_("%u: DELETED (%u)\n", i, hash(buf[i].key));
        } else {
            //DUMP_("%u: \"%s\" (%u) -> \"%s\"\n", i, buf[i].key, hash(buf[i].key), buf[i].value);
            DUMP_("%u\n", hash(buf[i].key));
        }
    }

    fclose(dfile);
}

const Hashtable::Node *Hashtable::debugGetBuf() const {
    return buf;
}

#undef DUMP_

}
