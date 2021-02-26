/*

���� ���� � ���������� ��������. ������� �� ������� ����������� �������� (��� ��������� ��������, ��� ������� �� ������ ������ ��������!), ������ ����� � ����� ���������� �� ���� ���� ������ ������.

��� ���������, ��� �������� ������, ������ ��� �������� ���� ������� ������ � ����� ���� �����������, ������������� ��������� (���������� �� ������� ����� ����� (x,y), (x\N{MINUS SIGN}1,y), (x,y\N{MINUS SIGN}1), (x+1,y), (x,y+1)). ���� �������� N �����, ������������� � ���� ������� ������� � ��������� � ����� ��� ������. �� ���� ���� �� �������� ���� �����, ������ ����� �����...

��������, �������� �� ������ ���������� ���� ���� �����.

������� ������
������ ������ �������� ����� �������� ��� ����� ����� W, H � N � ������ � ������ ������ � ����������� (1\N{LESS-THAN OR SLANTED EQUAL TO}W,H\N{LESS-THAN OR SLANTED EQUAL TO}109) � ���������� �����, ������������ ����� (0\N{LESS-THAN OR SLANTED EQUAL TO}N\N{LESS-THAN OR SLANTED EQUAL TO}1000000). ��������� N ����� �������� �� ��� ����� ����� xi,yi ������ � ���������� �������� ���������� i-� ������ (\N{MINUS SIGN}109\N{LESS-THAN OR SLANTED EQUAL TO}xi,yi\N{LESS-THAN OR SLANTED EQUAL TO}109). ������ (x,y) ��������� �� ������, ���� 1\N{LESS-THAN OR EQUAL TO}x\N{LESS-THAN OR EQUAL TO}W � 1\N{LESS-THAN OR EQUAL TO}y\N{LESS-THAN OR EQUAL TO}H. ����� �����, ����������� ��� ������, �� �����������.

�������� ������
�������� �Yes�, ���� ���� �������� ���� �����, � �No� � ��������� ������.

������
������� ������
2 2 2
1 1
2 2
�������� ������
Yes

*/


#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cmath>


const long long LIMIT_CHECK = 5000000;

template <typename T>
inline bool checkInBox(T h, T w, T x, T y) {
    return 0 <= x && x < h && 0 <= y && y < w;
    //return (((h - x) ^ (x - 1)) & ((w - y) ^ (y - 1))) & (1 << ((sizeof(T) << 3) - 1));
}

template <typename T>
inline void setBit(unsigned *buf, T h, T w, T x, T y) {
    if (!checkInBox(h, w, x, y))
        return;

    long long offset = x * h + y;

    if (offset >= LIMIT_CHECK)
        return;

    unsigned byteOffset = offset >> 5;
    unsigned bitOffset = offset & 0b11111;

    //printf("><> %lld %lld %lld %lld 0x%08x\n", h, w, x, y, buf[byteOffset]);

    buf[byteOffset] |= 1 << bitOffset;

    //printf(">>> %lld %lld %lld %lld 0x%08x\n", h, w, x, y, buf[byteOffset]);
}


int main() {
    unsigned n = 0;
    long long w = 0, h = 0;

    int res = scanf("%lld %lld %u", &w, &h, &n);
    assert(res == 3);

    static_assert(sizeof(unsigned) == 4);  // I kinda need it

    //unsigned *buf = (unsigned *)calloc(((h * w) >> 5) + 1, sizeof(unsigned));
    unsigned *buf = (unsigned *)calloc((LIMIT_CHECK >> 5) + 1, sizeof(unsigned));

    while (n--) {
        long long x = 0, y = 0;

        res = scanf("%lld %lld", &x, &y);
        assert(res == 2);

        #define PAINT_(X, Y) \
            setBit(buf, h, w, (X) - 1, (Y) - 1);

        PAINT_(x, y);
        PAINT_(x + 1, y);
        PAINT_(x, y + 1);
        PAINT_(x - 1, y);
        PAINT_(x, y - 1);

        #undef PAINT_
    }

    unsigned size = h * w >> 5;
    if ((LIMIT_CHECK >> 5) < size)
        size = LIMIT_CHECK >> 5;

    for (unsigned i = 0; i < size; ++i) {
        //printf("0x%08x\n", buf[i]);

        if (buf[i] != (unsigned)-1 || (
                i == h * w >> 5 &&
                ((~(((unsigned)-1) << (h * w & 0b11111))) & (~buf[i])) != 0
            )) {
            printf("No\n");

            return 0;
        }
    }

    printf("Yes\n");

    free(buf);

    return 0;
}


/*

Obviously the best hashtable implementation is direct access)
And the trick is that you can't populate all the squares in the given time anyway,
so we don't have to keep track of all of them as well. We may assume with a
sufficiently small margin of error that the canvas being filled is equivalent to
the first LIMIT_CHECK cells being filled. So yes, that's convenient)

*/

