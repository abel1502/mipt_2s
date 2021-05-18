#ifndef VECTOR_H
#define VECTOR_H

#include "general.h"

#include <cstdlib>
#include <cstring>
#include <cassert>


namespace abel {

template <typename T>
class Vector {
public:

    using iterator = T *;
    using const_iterator = const T *;

    static const unsigned DEFAULT_CAPACITY = 2;

    FACTORIES(Vector<T>);

    bool ctor() {
        buf = nullptr;

        size = 0;
        capacity = 0;

        TRY_B(resize(DEFAULT_CAPACITY));

        return false;
    }

    bool ctor(unsigned new_size) {
        TRY_B(ctor());

        for (unsigned i = 0; i < new_size; ++i) {
            TRY_B(append());
        }

        return false;
    }

    void dtor() {
        if constexpr (HAS_FACTORIES(T)) {
            if (buf) {
                for (unsigned i = 0; i < size; ++i) {
                    buf[i].dtor();
                    //buf[i] = {};
                }
            }
        }

        free(buf);
        buf = nullptr;

        size = 0;
        capacity = 0;
    }

    T &operator[](unsigned ind) {
        assert(size);
        ind = (ind + size) % size;

        assert(ind < size);

        assert(buf);

        return buf[ind];
    }

    const T &operator[](unsigned ind) const {
        assert(size);
        ind = (ind + size) % size;  // Despite being unsigned, ind would still work correctly if it's negative

        assert(ind < size);

        assert(buf);

        return buf[ind];
    }

    bool append() {
        if (size >= capacity) {
            TRY_B(resize(capacity * 2));

            assert(capacity > size);
        }

        if constexpr (HAS_FACTORIES(T)) {
            TRY_B(buf[size].ctor());
        } else {
            buf[size] = {};
        }

        size++;

        return false;
    }

    bool append(const T &value) {
        if (size >= capacity) {
            TRY_B(resize(capacity * 2));

            assert(capacity > size);
        }

        buf[size++] = value;

        return false;
    }

    bool extend(unsigned count) {
        while (size + count > capacity) {
            TRY_B(resize(capacity * 2));
        }

        for (unsigned i = size; i < size + count; ++i) {
            if constexpr (HAS_FACTORIES(T)) {
                TRY_B(buf[i].ctor());
            } else {
                buf[i] = {};
            }
        }

        size += count;

        return false;
    }

    const T &pop() {
        assert(!isEmpty());

        assert(buf);

        return buf[--size];
    }

    bool isEmpty() const {
        return size == 0;
    }

    unsigned getSize() const {
        return size;
    }

    unsigned getCapacity() const {
        return capacity;
    }

    const T *getBuf() const {
        return buf;
    }

    const_iterator begin() const {
        return buf;
    }

    iterator begin() {
        return buf;
    }

    const_iterator end() const {
        return buf + size;
    }

    iterator end() {
        return buf + size;
    }

private:

    T *buf;

    unsigned size;
    unsigned capacity;

    bool resize(unsigned new_capacity) {
        assert(new_capacity >= capacity);

        if (new_capacity == 0)
            new_capacity = DEFAULT_CAPACITY;

        T *newBuf = (T *)realloc(buf, new_capacity * sizeof(T));
        TRY_B(!newBuf);

        buf = newBuf;

        capacity = new_capacity;

        return false;
    }

};


template <typename T, unsigned COMPACT=1>
class CompactVector {
public:

    static_assert(sizeof(T) <= sizeof(void *));

    FACTORIES(CompactVector<T>);

    bool ctor() {
        buf = nullptr;

        size = 0;
        capacity = 0;

        return false;
    }

    void dtor() {
        if constexpr (HAS_FACTORIES(T)) {
            if (isCompact()) {
                for (unsigned i = 0; i < size; ++i) {
                    compactBuf[i].dtor();
                }
            } else {
                if (buf) {
                    for (unsigned i = 0; i < size; ++i) {
                        buf[i].dtor();
                    }
                }
            }
        }

        if (!isCompact()) {
            free(buf);
            buf = nullptr;
        }

        size = 0;
        capacity = 0;
    }

    T &operator[](unsigned ind) {
        assert(size);
        ind = (ind + size) % size;

        assert(ind < size);

        if (isCompact()) {
            return compactBuf[ind];
        }

        assert(buf);

        return buf[ind];
    }

    const T &operator[](unsigned ind) const {
        assert(size);
        ind = (ind + size) % size;

        assert(ind < size);

        if (isCompact()) {
            return compactBuf[ind];
        }

        assert(buf);

        return buf[ind];
    }

    // I guess compact vector can exist without an empty append

    bool append(const T &value) {
        if (isCompact()) {
            if (size + 1 <= COMPACT) {
                compactBuf[size++] = value;

                return false;
            }

            capacity = COMPACT + 1;
            T *tmpBuf = (T *)calloc(capacity, sizeof(T));
            TRY_B(!tmpBuf);

            for (unsigned i = 0; i < COMPACT; ++i) {
                tmpBuf[i] = compactBuf[i];
                compactBuf[i] = {};
            }

            buf = tmpBuf;
        }

        if (size >= capacity) {
            TRY_B(resize(capacity * 2));

            assert(capacity > size);
        }

        buf[size++] = value;

        return false;
    }

    const T &pop() {
        assert(!isEmpty());

        if (isCompact())
            return compactBuf[--size];

        assert(buf);

        return buf[--size];
    }

    bool isEmpty() const {
        return size == 0;
    }

    unsigned getSize() const {
        return size;
    }

    unsigned getCapacity() const {
        return isCompact() ? COMPACT : capacity;
    }

    constexpr bool isCompact() const {
        return capacity == 0;
    }

private:

    union {
        T *buf;

        T compactBuf[COMPACT];
    };

    unsigned size;
    unsigned capacity;

    bool resize(unsigned new_capacity) {
        assert(capacity != 0);
        assert(new_capacity >= capacity);

        T *newBuf = (T *)realloc(buf, new_capacity * sizeof(T));
        TRY_B(!newBuf);

        buf = newBuf;
        capacity = new_capacity;

        return false;
    }

};

}


#endif // VECTOR_H
