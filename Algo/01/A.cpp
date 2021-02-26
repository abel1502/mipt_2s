/*

¬аша задача Ч написать программу, моделирующую простое устройство, которое умеет прибавл€ть целые значени€ к целочисленным переменным.

¬ходные данные
¬ходной файл состоит из одной или нескольких строк, описывающих операции. —трока состоит из названи€ переменной и числа, которое к этой переменной надо добавить. ¬се числа не превосход€т 100 по абсолютной величине. »значально все переменные равны нулю. Ќазвани€ переменных состо€т из не более чем 100000 маленьких латинских букв. –азмер входного файла не превосходит 2 мегабайта.

¬ыходные данные
ƒл€ каждой операции выведите на отдельной строке значение соответствующей переменной после выполнени€ операции.

ѕример
входные данные
a 2
b 3
a -1
c 4
b 17
xyz 23
выходные данные
2
3
1
4
20
23

*/



#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>


namespace abel {

    typedef unsigned long long hash_t;

    const hash_t BASE = 299;
    const hash_t MOD = 1000000007;

    hash_t hash(const char *str) {
        assert(str);

        hash_t h = 0;

        while (*str) {
            h = (h * BASE + *str) % MOD;
            str++;
        }

        return h;
    }


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
        char *key;
        long long value;

        Pair() :
            key{nullptr},
            value{0} {}

        Pair(const Pair &other) :
            Pair{other.key, other.value} {}

        Pair(const char *new_key, long long new_value) :
            key{nullptr},
            value{new_value} {

            assert(new_key);

            key = new char[strlen(new_key) + 1];

            strcpy(key, new_key);
        }

        ~Pair() {
            delete key;
        }

        bool operator==(const Pair &other) const {
            return strcmp(key, other.key) == 0;
        }

        hash_t hash() const {
            return abel::hash(key);
        }
    };

}


int main() {
    abel::Hashtable<abel::Pair> ht{};
    char key[100001] = "";
    long long value = 0;

    while (scanf("%100000s", key) == 1) {
        int res = scanf("%lld", &value);
        assert(res == 1);

        abel::Pair *item = ht.set({key, 0});
        item->value += value;
        printf("%lld\n", item->value);
    }

    return 0;
}


/*

Plan:
1) Make a shitty hashtable
2) ...?
3) PROFIT

*/


