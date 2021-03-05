/*

Даны две строки, состоящих из маленьких латинских букв. Нужно найти их наибольшую общую подпоследовательность.

Входные данные
На первой строке первая строка. На второй строке вторая строка. Длины строк от 1 до 5000.

Выходные данные
Максимальную по длине общую подпоследовательность на отдельной строке. Если ответов несколько, выведите любой. Если ответ пуст, перевод строки выводить все равно нужно.

Пример
входные данные
abacabadabacaba
dbdccdbd
выходные данные
bcbd

*/


#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <new>


class String {
public:
    String() :
        size{0},
        capacity{1},  // Buf always contains a trailing null byte
        buf{new char[capacity]{}} {}

    String(unsigned defaultCapacity) :
        size{0},
        capacity{defaultCapacity},
        buf{new char[capacity]{}} {}

    String(const char *src) :
        size{(unsigned)strlen(src)},
        capacity{size + 1},
        buf{new char[capacity]{}} {

        strcpy(buf, src);
    }

    String(const String &other) :
        String(other.buf) {}

    String &operator=(const String &other) {
        if (buf && buf != other.buf) {
            delete[] buf;
        }

        return *this = String(other);
    }

    ~String() {
        delete[] buf;
        buf = nullptr;
        size = 0;
        capacity = 0;
    }

    unsigned getSize() {
        return size;
    }

    unsigned getCapacity() {
        return capacity;
    }

    const char *getRaw() {
        return buf;
    }

    char &operator[](size_t index) {
        assert(index < capacity);  // index >= size will return null byte

        return buf[index];
    }

    char operator[](size_t index) const {
        assert(index < capacity);

        return buf[index];
    }

    void scan() {
        int cur = 0;

        while ((cur = fgetc(stdin)) != EOF && cur != '\n') {
            if (cur == '\r')
                continue;

            if (size + 1 >= capacity) {
                assert(capacity > 0);

                resize(capacity * 2);
            }

            buf[size++] = cur;
        }
    }

    void print() {
        printf("%s", buf);
    }

    void println() {
        printf("%s\n", buf);
    }

    void adjustSize() {
        size = (unsigned)strlen(buf);
    }

private:
    unsigned size;
    unsigned capacity;
    char *buf;

    void resize(unsigned newCapacity) {
        assert(newCapacity > capacity);

        char *newBuf = (char *)realloc(buf, newCapacity * sizeof(buf[0]));
        if (!newBuf)
            throw std::bad_alloc();

        buf = newBuf;
        capacity = newCapacity;
    }

};


struct Item {
    unsigned value;

    enum {
        NONE,
        LEFT,
        RIGHT,
        COMMON
    } __attribute__((__packed__)) parentType;

    static_assert(sizeof(parentType) == 1);
};


int main() {
    String a{5001}, b{5001};  // Without preset capacities it might go as high as 8192 due to 2-degree resizes

    a.scan();
    b.scan();

    unsigned n = a.getSize();
    unsigned m = b.getSize();

    unsigned dpHeight = n + 1;
    unsigned dpWidth = m + 1;

    // Wow, I actually just found out new[] doesn't zero everything out by default...
    Item *dp = new Item[dpHeight * dpWidth] {};

    #define DP_(X, Y)    dp[(X) * dpWidth + (Y)]

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j = 0; j < m; ++j) {
            if (a[i] == b[j]) {
                DP_(i + 1, j + 1) = {DP_(i, j).value + 1, Item::COMMON};
            } else if (DP_(i, j + 1).value > DP_(i + 1, j).value) {
                DP_(i + 1, j + 1) = {DP_(i, j + 1).value, Item::LEFT};
            } else {
                DP_(i + 1, j + 1) = {DP_(i + 1, j).value, Item::RIGHT};
            }
        }
    }

    // b.~String();
    // This somehow causes a WA 2

    // String result{DP_(n, m).value + 1};
    // Due to a memory limit answer will now be stored in b
    // (I consider the person who decided to set a 16-kb memory limit incredibly dumb)
    // (WAIT A SECOND - now that I see how the task claims "16 mb" instead, I can say
    //  they're also a liar, and are terrible even at that. I despite them already...)

    unsigned resPos = DP_(n, m).value - 1;
    b[resPos + 1] = '\0';
    b.adjustSize();

    unsigned dpI = n;
    unsigned dpJ = m;
    bool finished = false;
    while (!finished) {
        switch (DP_(dpI, dpJ).parentType) {
        case Item::LEFT:
            dpI--;
            break;

        case Item::RIGHT:
            dpJ--;
            break;

        case Item::COMMON:
            dpI--;
            dpJ--;
            b[resPos--] = a[dpI];
            break;

        case Item::NONE:
            finished = true;
            break;

        default:
            assert(false);
            break;
        }
    }

    assert(resPos == (unsigned)-1);

    b.println();

    #undef DP_

    delete[] dp;

    return 0;
}


/*

Same as F, essentially

*/
