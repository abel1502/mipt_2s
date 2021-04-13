#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "general.h"
#include "filebuf.h"
#include "checksum.h"


namespace abel {

#pragma warning(push)
#pragma warning(disable : 26812 /* Suggest class enum over enum */)
class Hashtable {
public:
    enum result_e {
        R_OK = 0,
        R_BADMEMORY,
        R_BADPTR,
        R_BADSIZE,
        R_NOTFOUND,
        R_NOTIMPL,    /// A very special result, saying the feature you're trying to use should be pesent, but isn't yet implemented
        // TODO
    };

    static constexpr unsigned DEFAULT_CAPACITY = 256;
    static constexpr unsigned KEY_LEN = 32;
    static constexpr unsigned CRITICAL_LOAD_FACTOR = 0x80;  // per 0x100
    static constexpr unsigned RESIZE_FACTOR = 2;

    using key_t    = char[KEY_LEN];
    using value_t  = const char *;
    using mvalue_t = char *;  // Mutable

    #ifdef __GNUC__
    static const value_t NODE_FREE;
    static const value_t NODE_DELETED;
    static const value_t NODE_SPECIAL;
    #else
    static constexpr value_t NODE_FREE    = (value_t)0;
    static constexpr value_t NODE_DELETED = (value_t)1;
    static constexpr value_t NODE_SPECIAL = (value_t)256;
    #endif

    struct Node {
        key_t key;
        value_t value;
        // TODO: Maybe hold a precomputed crc32 here for fast rehashing
    };

    FACTORIES(Hashtable)

    result_e ctor();

    result_e ctor(unsigned new_capacity);

    result_e ctor(FileBuf *src);

    void dtor();

    void swap(Hashtable &other);

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

    result_e validate() const;

    /// @param [out] key_t
    static void castToKey(const char *src, key_t key);

    result_e set(const key_t key, value_t value);

    result_e get(const key_t key, mvalue_t *value) const;  // TODO: const_cast

    mvalue_t get(const key_t key) const;

    result_e del(const key_t key);

    result_e pop(const key_t key, mvalue_t *value);

    mvalue_t pop(const key_t key);

    result_e maybeFlushDeletions();

    result_e resize(unsigned new_capacity);

    void dump() const;

private:
    mutable result_e lastResult;

    unsigned capacity;
    unsigned size;
    unsigned deleted;

    Node *buf;

    inline unsigned hash(const key_t key) const {
        //return crc32_compute(key, KEY_LEN) % capacity;  // On average 1140 items per bucket
        //return (0x61C88647ULL * (unsigned long long)crc32_compute(key, KEY_LEN) >> 32) % capacity;  // Exactly the same, indicating it was a problem with crc itself. I guess it shouldn't be used in hashtables
        return fnv1a_64(key, KEY_LEN) % (unsigned long long)capacity;  // 1.5 per bucket. One. Point. Five. I don't think any intrinsicness of crc32 can compensate for this. (Although I'll still test it)
        
        // TODO: If I stick to fnv1a_64, I could as well remove the key length limitation due to redundancy. It would probably only speed things up.
    }
};
#pragma warning(pop)

}

#endif