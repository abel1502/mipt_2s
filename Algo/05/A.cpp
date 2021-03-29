/*

Вершина графа u называется инцидентной ребру e, если u является одним из концов ребра e.

Аналогично, ребро e называется инцидентным вершине u, если один из концов e — это вершина u.

Матрицей инцидентности графа G=(V,E) называется прямоугольная таблица из |V| строк и |E| столбцов, в которой на пересечении i-й строки и j-го столбца записана единица, если вершина i инцидентна ребру j, и ноль в противном случае.

Дан неориентированный граф. Выведите его матрицу инцидентности.

Входные данные
В первой строке входного файла заданы числа N и M через пробел — количество вершин и рёбер в графе, соответственно (1\N{LESS-THAN OR SLANTED EQUAL TO}N\N{LESS-THAN OR SLANTED EQUAL TO}100, 0\N{LESS-THAN OR SLANTED EQUAL TO}M\N{LESS-THAN OR SLANTED EQUAL TO}10000). Следующие M строк содержат по два числа ui и vi через пробел (1\N{LESS-THAN OR SLANTED EQUAL TO}ui,vi\N{LESS-THAN OR SLANTED EQUAL TO}N); каждая такая строка означает, что в графе существует ребро между вершинами ui и vi. Рёбра нумеруются в том порядке, в котором они даны во входном файле, начиная с единицы.

Выходные данные
Выведите в выходной файл N строк, по M чисел в каждой. j-й элемент i-й строки должен быть равен единице, если вершина i инцидентна ребру j, и нулю в противном случае. Разделяйте соседние элементы строки одним пробелом.

Пример
входные данные
3 2
1 2
2 3
выходные данные
1 0
1 1
0 1

*/

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>
#include <algorithm>


int main() {
    unsigned n = 0, m = 0;
    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    std::vector<std::vector<bool>> incidence(n, std::vector<bool>(m, false));

    for (unsigned i = 0; i < m; ++i) {
        unsigned u = 0, v = 0;
        res = scanf("%u %u", &u, &v);
        assert(res == 2);

        incidence[u - 1][i] = incidence[v - 1][i] = true;
    }

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j = 0; j < m; ++j) {
            printf("%u ", (unsigned)incidence[i][j]);
        }

        printf("\n");
    }

    return 0;
}


/*

Do I really need to explain anything?

*/
