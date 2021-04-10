#ifndef FILEBUF_H
#define FILEBUF_H

#include "general.h"

#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>

namespace abel {

class FileBuf {
public:
    FACTORIES(FileBuf);

    bool ctor();

    /// File read constructor
    bool ctor(const char *name, const char *mode);

    /// Memory read constructor
    bool ctor(const char *src, size_t amount);

    /// Memory read constructor with implicit amount
    bool ctor(const char *src);

    void dtor();

    size_t getSize() const;

    size_t getLineCnt() const;

    const char *getData() const;

    bool isInited() const;

private:
    size_t size;
    size_t lineCnt;
    char *buf;
};


class FileBufIterator {
public:
    FACTORIES(FileBufIterator);

    bool ctor();

    bool ctor(const FileBuf *new_buf);

    void dtor();

    char cur() const;

    char peek(int offset) const;

    char next();

    char prev();

    void skipSpace();

    bool isEof() const;

    size_t getPos() const;

    const char *getPtr() const;

private:
    const FileBuf *buf;
    size_t pos;

};

}

#endif // FILEBUF_H
