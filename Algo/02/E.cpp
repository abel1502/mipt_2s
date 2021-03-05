/*

Группа математиков проводит бои между натуральными числами. Результаты боя между двумя натуральными числами, вообще говоря, случайны, однако подчиняются следующему правилу: если одно из чисел не менее чем в два раза превосходит другое, то большее число всегда побеждает; в противном случае победить может как одно, так и другое число.

Бой называется неинтересным, если его результат предопределён. Множество натуральных чисел называется мирным, если бой любой пары различных чисел из этого множества неинтересен. Силой множества называется сумма чисел в нём. Сколько существует мирных множеств натуральных чисел силы n?

Входные данные
В первой строке входного файла задано число n (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}2000).

Выходные данные
В первой строке выходного файла выведите одно число — количество мирных множеств натуральных чисел силы n.

Примеры
входные данные
2
выходные данные
1
входные данные
5
выходные данные
2

*/


#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>


int main() {
    typedef unsigned long long val_t;

    unsigned n = 0;
    int res = scanf("%u", &n);
    assert(res == 1);

    std::vector<std::vector<val_t>> dp(n + 1, std::vector<val_t>(n + 1, 0));
    dp[0][0] = 1;

    for (unsigned power = 1; power <= n; ++power) {
        for (unsigned max = 0; max <= power; ++max) {
            for (unsigned other = 0; other * 2 <= max; ++other) {
                dp[power][max] += dp[power - max][other];
            }
        }
    }

    val_t sum = 0;

    for (unsigned i = 0; i <= n; ++i) {
        sum += dp[n][i];
    }

    printf("%llu\n", sum);

    return 0;
}


/*

dp[power][max] holds the number of peaceful sets of `power` power and with `max` as max.
My groupmates shared the incredibly valuable info that O(n**3) actually works - so why not?

*/


