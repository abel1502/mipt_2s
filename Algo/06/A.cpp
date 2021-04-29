/*

На шахматной доске размером 8\N{MULTIPLICATION SIGN}8 заданы две клетки. Соедините эти клетки кратчайшим путем коня.

Входные данные
Программа получает на вход координаты двух клеток, каждая в отдельной строке. Координаты клеток задаются в виде буквы (от "a" до "h") и цифры (от 1 до 8) без пробелов.

Выходные данные
Программа должна вывести путь коня, начинающийся и заканчивающийся в данных клетках и содержащий наименьшее число клеток.

Пример
входные данные
a1
h8
выходные данные
a1
c2
a3
b5
d6
f7
h8

*/


#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>
#include <queue>
#include <algorithm>
#include <cctype>


struct Point {
    static const Point BAD;

    unsigned x;
    unsigned y;

    Point() :
        x{0},
        y{0} {}

    Point(unsigned new_x, unsigned new_y) :
        x{new_x},
        y{new_y} {}

    Point(const Point &other) = default;
    Point(Point &&other) = default;
    Point &operator=(const Point &other) = default;
    Point &operator=(Point &&other) = default;

    void read() {
        char tmp = 0;

        int res = scanf(" %c%u", &tmp, &y);
        assert(res == 2);
        assert(isalpha(tmp));

        x = tolower(tmp) - 'a';
        y--;
    }

    void write() const {
        printf("%c%u", x + 'a', y + 1);
    }

    friend bool operator==(const Point &a, const Point &b) {
        return a.x == b.x && a.y == b.y;
    }

    friend bool operator!=(const Point &a, const Point &b) {
        return !(a == b);
    }

    Point knightMove(unsigned idx) const {
        constexpr int dx[] = {1, 2, -1,  2,  1, -2, -1, -2};
        constexpr int dy[] = {2, 1,  2, -1, -2,  1, -2, -1};

        assert(idx < 8);

        return {x + dx[idx], y + dy[idx]};
    }

    bool isValid() const {
        return x < 8 && y < 8;
    }

    static void bfs(const Point &from, const Point &to, std::vector<std::vector<Point>> &parents) {
        parents.assign(8, std::vector<Point>(8, BAD));
        std::queue<Point> toVisit{};
        toVisit.push(from);

        while (!toVisit.empty()) {
            Point cur = toVisit.front();
            toVisit.pop();

            if (cur == to)
                return;

            for (unsigned i = 0; i < 8; ++i) {
                Point next = cur.knightMove(i);

                if (!(next.isValid() && parents[next.x][next.y] == BAD))
                    continue;

                parents[next.x][next.y] = cur;
                toVisit.push(next);
            }
        }

        assert(false);
    }
};

const Point Point::BAD{-1u, -1u};


int main() {
    Point start{}, finish{};

    start.read();
    finish.read();

    std::vector<std::vector<Point>> parents{};
    Point::bfs(start, finish, parents);



    std::vector<Point> path{};
    while (finish != Point::BAD && finish != start) {
        path.push_back(finish);

        finish = parents[finish.x][finish.y];
    }

    path.push_back(start);

    reverse(path.begin(), path.end());

    for (const Point &cur : path) {
        cur.write();
        puts("");
    }

    return 0;
}


/*

BFS

*/
