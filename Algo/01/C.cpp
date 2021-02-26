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


namespace abel {

    typedef unsigned long long hash_t;


    template <typename T>
    class HTNode {
    public:
        T value;
        HTNode *next;
        HTNode *prev;

        HTNode() :
            value{},
            next{nullptr},
            prev{nullptr} {}

        // TODO: Rule of 5

        HTNode(const T &new_value) :
            value{new_value},
            next{nullptr},
            prev{nullptr} {}

        HTNode(const T &new_value, HTNode<T> *parent) :
            value{T{new_value}},
            next{parent},
            prev{nullptr} {

            if (parent)
                parent->prev = this;
        }

        ~HTNode() {
            if (next)
                delete next;
        }

        HTNode<T> *erase() {
            if (prev)
                prev->next = next;
            if (next)
                next->prev = prev;

            HTNode<T> *replacement = next;

            delete this;

            return replacement;
        }

    };


    template <typename T>
    class Hashtable {
    public:
        static const unsigned DEFAULT_CAPACITY = 1553333;

        // TODO: Rule of 5
        Hashtable() :
            Hashtable{DEFAULT_CAPACITY} {}

        Hashtable(unsigned new_capacity) :
            table{new HTNode<T> *[new_capacity]{}},
            size{0},
            capacity{new_capacity} {}

        ~Hashtable() {
            for (unsigned i = 0; i < capacity; ++i) {
                if (table[i]) {
                    delete table[i];
                }
            }

            delete table;
        }

        T *get(const T &key) {
            hash_t h = key.hash() % capacity;

            HTNode<T> *cur = table[h];

            while (cur) {
                if (cur->value == key) {
                    return &cur->value;
                }

                cur = cur->next;
            }

            return nullptr;
        }

        T *set(const T &key) {
            hash_t h = key.hash() % capacity;

            HTNode<T> *cur = table[h];

            while (cur) {
                if (cur->value == key) {
                    // TODO: update value?

                    return &cur->value;
                }

                cur = cur->next;
            }

            size++;
            table[h] = new HTNode<T>(key, table[h]);

            return &table[h]->value;
        }

        unsigned getSize() {
            return size;
        }

        unsigned getCapacity() {
            return capacity;
        }

    private:
        HTNode<T> **table;
        unsigned size;
        unsigned capacity;
    };


    class Point {
    public:
        long long x;
        long long y;

        Point() :
            x{0},
            y{0} {}

        Point(const Point &other) :
            Point{other.x, other.y} {}

        Point(long long new_x, long long new_y) :
            x{new_x},
            y{new_y} {}

        bool operator==(const Point &other) const {
            return x == other.x && y == other.y;
        }

        hash_t hash() const {
            return y * //((y << (sizeof(long long) >> 1)) | x);
        }
    };

}


template <typename T>
constexpr bool fastCheckInBox(T h, T w, T x, T y) {
    return 1 <= x && x <= h && 1 <= y && y <= w;
    //return (((h - x) ^ (x - 1)) & ((w - y) ^ (y - 1))) & (1 << ((sizeof(T) << 3) - 1));
}


int main() {
    abel::Hashtable<abel::Point> ht{};

    unsigned n = 0;
    long long w = 0, h = 0;

    int res = scanf("%lld %lld %u", &w, &h, &n);
    assert(res == 3);

    for (; n > 0; --n) {
        long long x = 0, y = 0;

        res = scanf("%lld %lld", &x, &y);
        assert(res == 2);

        //abel::Point tmp{x, y};
        //printf(">>> %llu\n", tmp.hash());

        #define PAINT_(X, Y) \
            if (fastCheckInBox(h, w, X, Y))  ht.set({X, Y});

        PAINT_(x, y);
        PAINT_(x + 1, y);
        PAINT_(x, y + 1);
        PAINT_(x - 1, y);
        PAINT_(x, y - 1);

        #undef PAINT_
    }

    if (ht.getSize() == w * h)
        printf("Yes\n");
    else
        printf("No\n");

    return 0;
}


/*

You probably get it without my explanations

*/
