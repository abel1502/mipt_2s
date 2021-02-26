/*

Пусть все натуральные числа исходно организованы в список в естественном порядке. Разрешается выполнить следующую операцию: swap(a,b). Эта операция возвращает в качестве результата расстояние в текущем списке между числами a и b и меняет их местами.

Задана последовательность операций swap. Требуется вывести в выходной файл результат всех этих операций.

Входные данные
Первая строка входного файла содержит число n (1\N{LESS-THAN OR SLANTED EQUAL TO}n\N{LESS-THAN OR SLANTED EQUAL TO}200000) — количество операций. Каждая из следующих n строк содержит по два числа в диапазоне от 1 до 109 — аргументы операций swap.

Выходные данные
Для каждой операции во входном файле выведите ее результат.

Пример
входные данные
4
1 4
1 3
4 5
1 4
выходные данные
3
1
4
2
Примечание
Хеш-таблицы из STL (unordered_map и т.д.) получат TL.

Придётся писать свою хеш-таблицу.

Самая быстрая и простая хеш-таблица – хеш-таблица с открытой адресацией.

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
        static const unsigned DEFAULT_CAPACITY = 1000000;

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

            delete[] table;
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

    private:
        HTNode<T> **table;
        unsigned size;
        unsigned capacity;
    };


    class Pair {
    public:
        long long key;
        long long value;

        Pair() :
            key{0},
            value{0} {}

        Pair(const Pair &other) :
            Pair{other.key, other.value} {}

        Pair(const long long new_key, long long new_value) :
            key{new_key},
            value{new_value} {}

        ~Pair() {}

        bool operator==(const Pair &other) const {
            return key == other.key;
        }

        hash_t hash() const {
            return key;
        }
    };

}


int main() {
    abel::Hashtable<abel::Pair> ht{};

    unsigned n = 0;
    int res = scanf("%u", &n);
    assert(res == 1);

    for (; n > 0; --n) {
        long long a = 0,
                  b = 0;

        res = scanf("%lld %lld", &a, &b);
        assert(res == 2);

        abel::Pair *itemA = ht.set({a, a});
        abel::Pair *itemB = ht.set({b, b});

        printf("%lld\n", abs(itemA->value - itemB->value));

        std::swap(itemA->value, itemB->value);  // Hopefully this is legal - Max claims it worked for him
    }

    return 0;
}


/*

Plan:
1) REUSE the same hashtable
2) Get awarded for garbage recycling
3) Double profit?

*/


