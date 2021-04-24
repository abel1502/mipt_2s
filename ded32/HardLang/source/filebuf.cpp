#include "filebuf.h"

#include <cassert>
#include <cctype>


namespace abel {

bool FileBuf::ctor() {
    size = 0;
    lineCnt = 0;
    buf = nullptr;

    return false;
}

bool FileBuf::ctor(const char *name, const char *mode) {
    assert(name);
    assert(mode);
    assert(!buf);

    FILE *file = fopen(name, mode);

    struct stat fbuf = {};
    size_t bytesRead = 0;
    lineCnt = 0;

    if (!file)
        goto error;

    if (fstat(fileno(file), &fbuf) != 0)
        goto error;

    size = (size_t)fbuf.st_size + 1;

    buf = (char *)calloc(size, 1);

    if (!buf)
        goto error;

    bytesRead = fread(buf, 1, size - 1, file);

    for (char *cur = buf; *cur; ++cur) {
        if (*cur == '\n') {
            lineCnt++;
        }
    }

    if (bytesRead != size - 1 && bytesRead + lineCnt != size - 1)
        goto error;


    return false;

error:
    free(buf);
    buf = nullptr;

    if (file) {
        fclose(file);
        file = nullptr;
    }

    return true;
}

bool FileBuf::ctor(const char *src, size_t amount) {
    assert(src);

    size = amount + 1;
    lineCnt = 0;

    buf = (char *)calloc(amount + 1, 1);

    if (!buf)
        goto error;

    memcpy(buf, src, amount);

    for (char *cur = buf; *cur; ++cur) {
        if (*cur == '\n') {
            lineCnt++;
        }
    }

    return false;

error:
    free(buf);
    buf = nullptr;

    return true;
}

bool FileBuf::ctor(const char *src) {
    return ctor(src, strlen(src));
}

void FileBuf::dtor() {
    free(buf);

    size = 0;
    lineCnt = 0;
    buf = nullptr;
}

size_t FileBuf::getSize() const {
    return size;
}

size_t FileBuf::getLineCnt() const {
    return lineCnt;
}

char *FileBuf::getData() {
    return buf;
}

const char *FileBuf::getData() const {
    return buf;
}

bool FileBuf::isInited() const {
    return buf != nullptr;
}

//================================================================================

bool FileBufIterator::ctor() {
    buf = nullptr;
    pos = 0;

    return false;
}

bool FileBufIterator::ctor(const FileBuf *new_buf) {
    assert(new_buf);

    buf = new_buf;
    pos = 0;

    return false;
}

void FileBufIterator::dtor() {
    buf = nullptr;
    pos = 0;
}

char FileBufIterator::cur() const {
    if (pos >= buf->getSize())
        return 0;

    return buf->getData()[pos];
}

char FileBufIterator::peek(int offset) const {
    size_t goal = pos + offset;  // if this cycles it into the negative, it will still
                                    // trigger the if, so extra checks are unnecessary
    if (goal >= buf->getSize())
        return 0;

    return buf->getData()[goal];
}

char FileBufIterator::next() {
    char tmp = cur();

    pos++;

    return tmp;
}

char FileBufIterator::prev() {
    char tmp = cur();

    pos--;  // Going below zero is pretty much synonymous to going out of bounds,
            // so we don't need extra checks for this either.

    return tmp;
}

void FileBufIterator::skipSpace() {
    while (isspace(cur())) {
        next();
    }
}

bool FileBufIterator::isEof() const {
    return !cur();
}

size_t FileBufIterator::getPos() const {
    return pos;
}

const char *FileBufIterator::getPtr() const {
    if (pos >= buf->getSize())
        return "\0";

    return buf->getData() + pos;
}

}

