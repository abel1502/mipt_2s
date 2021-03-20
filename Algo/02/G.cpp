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
#include <vector>


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

    unsigned getSize() const {
        return size;
    }

    unsigned getCapacity() const {
        return capacity;
    }

    const char *getRaw() const {
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

    void print() const {
        printf("%s", buf);
    }

    void println() const {
        printf("%s\n", buf);
    }

    void adjustSize() {
        size = (unsigned)strlen(buf);
    }

    static void getLcs(std::vector<unsigned> &dp, const char *a, const char *b, unsigned aStart, unsigned aEnd, unsigned bStart, unsigned bEnd) {
        bool aRev = false;
        bool bRev = false;

        if (aStart > aEnd) {
            aRev = true;
            std::swap(aStart, aEnd);
            //aStart++;
            //aEnd++;
        }

        if (bStart > bEnd) {
            bRev = true;
            std::swap(bStart, bEnd);
            //bStart++;
            //bEnd++;
        }

        unsigned aLen = aEnd - aStart;
        unsigned bLen = bEnd - bStart;
        dp.assign(aLen + 1, 0);
        std::vector<unsigned> dp2(aLen + 1, 0);

        for (unsigned i = 0; i < bLen; ++i) {
            for (unsigned j = 0; j < aLen; ++j) {
                char aVal = 0;
                char bVal = 0;

                if (aRev)
                    aVal = a[aEnd - 1 - j];
                else
                    aVal = a[aStart + j];

                if (bRev)
                    bVal = b[bEnd - 1 - i];
                else
                    bVal = b[bStart + i];

                if (aVal == bVal) {
                    dp2[j + 1] = dp[j] + 1;
                }

                dp2[j + 1] = std::max(dp[j + 1], dp2[j + 1]);
                dp2[j + 1] = std::max(dp2[j + 1], dp2[j]);
            }

            dp.swap(dp2);
        }

        /*printf("DP: ");
        for (unsigned i = 0; i < aLen + 1; ++i) {
            printf("%u ", dp[i]);
        }printf("\n");*/
    }

    void hirschberg(const char *a, const char *b, unsigned aStart, unsigned aEnd, unsigned bStart, unsigned bEnd) {
        //printf("> [%u %u) [%u %u)\n", aStart, aEnd, bStart, bEnd);
        static std::vector<unsigned> lcs1, lcs2;

        /*printf(">\n");
        for (unsigned i = aStart; i < aEnd; ++i)
            printf("%c", a[i]);
        printf("\n");
        for (unsigned i = bStart; i < bEnd; ++i)
            printf("%c", b[i]);
        printf("\n");*/

        if (bStart == bEnd)
            return;

        if (aStart + 1 == aEnd) {
            for (unsigned i = bStart; i < bEnd; ++i) {
                if (b[i] == a[aStart]) {
                    (*this)[size++] = a[aStart];
                    break;
                }
            }
            return;
        }

        unsigned aMid = (aStart + aEnd) / 2;
        getLcs(lcs1, b, a, bStart, bEnd, aStart, aMid);
        getLcs(lcs2, b, a, bEnd, bStart, aEnd, aMid);  // TODO: Maybe do +1 for reversed

        /*printf(">> ");
        for (unsigned i = 0; i < lcs1.size(); ++i) {
            printf("%u ", lcs1[i]);
        } printf("\n");*/

        unsigned max = 0;
        unsigned maxInd = 0;
        unsigned bLen = bEnd - bStart;
        for (unsigned i = 0; i <= bLen; ++i) {
            if (lcs1[i] + lcs2[bLen - i] >= max) {
                max = lcs1[i] + lcs2[bLen - i];
                maxInd = bStart + i;
            }
        }

        hirschberg(a, b, aStart, aMid, bStart, maxInd);
        hirschberg(a, b, aMid, aEnd, maxInd, bEnd);
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


int main() {
    String a{5001}, b{5001}, ans{5001};

    a.scan();
    b.scan();

    unsigned n = a.getSize();
    unsigned m = b.getSize();

    ans.hirschberg(a.getRaw(), b.getRaw(), 0, n, 0, m);
    ans.println();

    return 0;
}


/*

I first tried to do the same as in F, but id didn't work.
There is a memory-efficient alternative - the Hirschberg algorithm,
so that's what I implemented

*/
