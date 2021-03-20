/*

Филипп любит гулять по своему городу, но знает, что шпионы из ЛесТеха могут быть где угодно, поэтому он хочет узнать, по скольким различным путям он может пройти по городу Долгопрудный. Город Филиппа состоит из всех точек (x,y) на плоскости таких, что x и y неотрицательны. Филипп должен начать прогулку в начале координат (точке (0,0)) и должен закончить путь в точке (k,0). Если Филипп сейчас находится в точке (x,y), то за один шаг он может перейти в точку (x+1,y+1),(x+1,y) или (x+1,y\N{MINUS SIGN}1).

Кроме того, существуют n горизонтальных отрезков, i-й из которых идет от точки x=ai до x=bi, включительно и располагается в y=ci. Гарантируется, что a1=0, an\N{LESS-THAN OR EQUAL TO}k\N{LESS-THAN OR EQUAL TO}bn, и ai=bi\N{MINUS SIGN}1 для всех 2\N{LESS-THAN OR EQUAL TO}i\N{LESS-THAN OR EQUAL TO}n. i-й из этих отрезков заставляет Филиппа находиться только в точках с y координатой в отрезке 0\N{LESS-THAN OR EQUAL TO}y\N{LESS-THAN OR EQUAL TO}ci, когда его x координата находится в отрезке ai\N{LESS-THAN OR EQUAL TO}x\N{LESS-THAN OR EQUAL TO}bi. Заметьте, что когда один отрезок кончается, а другой начинается, то он должен находиться под обоими отрезками одновременно.

Филипп хочет узнать, сколько существует различных путей (последовательностей шагов) из начала координат в точку (k,0), удовлетворяющих этим ограничениям, по модулю 109+7.

Входные данные
Первая строка содержит два целых числа n и k (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}100,1\N{LESS-THAN OR EQUAL TO}k\N{LESS-THAN OR EQUAL TO}1018) — число отрезков и x координата точки назначения.

Следующие n строк содержат по три целых числа ai, bi и ci(0\N{LESS-THAN OR EQUAL TO}ai<bi\N{LESS-THAN OR EQUAL TO}1018,0\N{LESS-THAN OR EQUAL TO}ci\N{LESS-THAN OR EQUAL TO}16) — левый и правый концы отрезка, и его y координата.

Гарантируется, что a1=0,an\N{LESS-THAN OR EQUAL TO}k\N{LESS-THAN OR EQUAL TO}bn, и ai=bi\N{MINUS SIGN}1 для всех 2\N{LESS-THAN OR EQUAL TO}i\N{LESS-THAN OR EQUAL TO}n.

Выходные данные
Выведите число путей, удовлетворяющих ограничениям, по модулю 109+7.

Примеры
входные данные
1 3
0 3 3
выходные данные
4
входные данные
2 6
0 3 0
3 10 2
выходные данные
4

*/

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <string>
#include <algorithm>
#include <vector>
#include <numeric>


namespace abel {


template <typename T>
class Matrix {
public:
    Matrix() :
        height{0},
        width{0},
        buf{} {}

    Matrix(unsigned new_height, unsigned new_width, const T &new_default = T{}) :
        height{new_height},
        width{new_width},
        buf(height, std::vector<T>(width, new_default)) {}

    Matrix(const Matrix<T> &other) :
        height{other.height},
        width{other.width},
        buf(other.buf) {}

    Matrix(unsigned new_height, unsigned new_width, const std::vector<std::vector<T>> &new_values) :
        height{new_height},
        width{new_width},
        buf(new_values) {

        assert(new_values.size() == new_height);

        for (unsigned i = 0; i < new_height; ++i) {
            assert(new_values[i].size() == new_width);
        }
    }

    /*Matrix(unsigned new_height, unsigned new_width, const std::vector<std::vector<T>> &&new_values) :
        height{new_height},
        width{new_width},
        buf(new_values) {

        assert(new_values.size() == new_height);

        for (unsigned i = 0; i < new_height; ++i) {
            assert(new_values[i].size() == new_width);
        }
    }*/

    Matrix<T> &operator=(const Matrix<T> &other) {
        height = other.height;
        width = other.width;
        buf = other.buf;

        return *this;
    }

    ~Matrix() {}

    static Matrix<T> E(unsigned size) {
        std::vector<std::vector<T>> tmp(size, std::vector<T>(size));

        for (unsigned i = 0; i < size; ++i) {
            tmp[i][i] = 1;
        }

        return Matrix(size, size, tmp);
    }

    T &operator()(unsigned row, unsigned column) {
        return buf[row][column];
    }

    const Matrix<T> &operator()(unsigned row, unsigned column) const {
        return buf[row][column];
    }

    bool operator==(const Matrix<T> &other) const {
        if (height != other.height || width != other.width)
            return false;

        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                if (buf[i][j] != other.buf[i][j])
                    return false;
            }
        }

        return true;
    }

    bool operator!=(const Matrix<T> &other) const {
        return !(*this == other);
    }

    Matrix<T> &operator+=(const Matrix<T> &other) {
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                buf[i][j] += other.buf[i][j];
            }
        }

        return *this;
    }

    friend Matrix<T> operator+(Matrix<T> left, const Matrix<T> &right) {
        return left += right;
    }

    Matrix<T> &operator-=(const Matrix<T> &other) {
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                buf[i][j] -= other.buf[i][j];
            }
        }

        return *this;
    }

    friend Matrix<T> operator-(Matrix<T> left, const Matrix<T> &right) {
        return left -= right;
    }

    Matrix<T> &operator-() {
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                buf[i][j] = -buf[i][j];
            }
        }

        return *this;
    }

    Matrix<T> &operator*=(T coeff) {
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                buf[i][j] *= coeff;
            }
        }

        return *this;
    }

    friend Matrix<T> operator*(Matrix<T> mat, const T &coeff) {
        return mat *= coeff;
    }

    friend Matrix<T> operator*(const T &coeff, Matrix<T> mat) {
        return mat *= coeff;
    }

    /*
    // Slow
    friend Matrix<T> operator*(const Matrix<T> &left, const Matrix<T> &right) {
        assert(left.width == right.height);

        Matrix<T> result(left.height, right.width);

        for (unsigned i = 0; i < left.height; ++i) {
            for (unsigned j = 0; j < right.width; ++j) {
                for (unsigned k = 0; k < left.width; ++k) {
                    result.buf[i][j] += left.buf[i][k] * right.buf[k][j];
                }
            }
        }

        return result;
    }
    */

    friend Matrix<T> operator*(const Matrix<T> &left, Matrix<T> &right) {
        assert(left.width == right.height);

        Matrix<T> result(left.height, right.width);

        right = ~right;

        for (unsigned i = 0; i < left.height; ++i) {
            for (unsigned j = 0; j < right.width; ++j) {
                for (unsigned k = 0; k < left.width; ++k) {
                    result.buf[i][j] += left.buf[i][k] * right.buf[j][k];
                }
            }
        }

        right = ~right;

        return result;
    }

    friend Matrix<T> operator*(const Matrix<T> &left, const Matrix<T> &right) {
        assert(left.width == right.height);

        Matrix<T> result(left.height, right.width);
        Matrix<T> tRight(right.height, right.width);

        tRight = ~right;

        for (unsigned i = 0; i < left.height; ++i) {
            for (unsigned j = 0; j < right.width; ++j) {
                for (unsigned k = 0; k < left.width; ++k) {
                    result.buf[i][j] += left.buf[i][k] * tRight.buf[j][k];
                }
            }
        }

        return result;
    }

    Matrix<T> &operator*=(const Matrix &other) {
        return *this = *this * other;
    }

    // Non-inplace transposition
    Matrix<T> operator~() const {
        Matrix<T> result(width, height);

        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                result.buf[j][i] = buf[i][j];
            }
        }

        return result;
    }

    Matrix<T> &sqrTranspose() {
        assert(isSqr());

        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = i + 1; j < width; ++j) {
                std::swap(buf[i][j], buf[j][i]);
            }
        }

        return *this;
    }

    Matrix<T> &operator%=(unsigned long long mod) {
        if (!mod)
            return *this;

        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                buf[i][j] %= mod;
            }
        }

        return *this;
    }

    friend Matrix<T> operator%(Matrix<T> mat, unsigned long long mod) {
        return mat %= mod;
    }

    Matrix<T> operator^(unsigned long long power) const {
        return pow(power);
    }

    Matrix<T> &operator^=(unsigned long long power) {
        return *this = pow(power);
    }

    bool isSqr() const {
        return height == width;
    }

    Matrix<T> pow(unsigned long long power, unsigned long long mod = 0) const {
        assert(isSqr());

        if (power == 0)
            return E(height);

        if (power == 1)
            return *this;

        if (power % 2 == 0) {
            Matrix<T> result = pow(power / 2, mod);
            return result.mul(result, mod);
        }

        return pow(power - 1, mod).mul(*this, mod);
    }

    Matrix<T> mul(const Matrix<T> &other, unsigned long long mod = 0) const {
        if (!mod)
            return *this * other;

        assert(width == other.height);

        Matrix<T> result(height, other.width);
        Matrix<T> tOther(other.height, other.width);

        tOther = ~other;

        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < other.width; ++j) {
                for (unsigned k = 0; k < width; ++k) {
                    result.buf[i][j] = (result.buf[i][j] + buf[i][k] * tOther.buf[j][k]) % mod;
                }
            }
        }

        return result;
    }

    T sum(unsigned long long mod = 0) const {
        T result = 0;

        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                result += buf[i][j];

                if (mod)
                    result %= mod;
            }
        }

        return result;
    }

    void print() const {
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                printf("%s ", std::to_string(buf[i][j]).c_str());
            }

            printf("\n");
        }
    }

    unsigned getHeight() const {
        return height;
    }

    unsigned getWidth() const {
        return width;
    }

private:
    unsigned height;
    unsigned width;

    std::vector<std::vector<T>> buf;
};


}


int main() {
    const unsigned MAX_HEIGHT = 17;
    const unsigned long long MOD = 1000000007;

    std::vector<abel::Matrix<unsigned long long>> transitions(MAX_HEIGHT, abel::Matrix<unsigned long long>(MAX_HEIGHT, MAX_HEIGHT, 0));
    for (unsigned i = 0; i < MAX_HEIGHT; ++i) {
        for (unsigned j = 0; j <= i; ++j) {
            if (j > 0) transitions[i](j, j - 1) = 1;
            if (1)     transitions[i](j, j)     = 1;
            if (j < i) transitions[i](j, j + 1) = 1;
        }
    }

    unsigned n = 0;
    unsigned long long k = 0;
    int res = scanf("%u %llu", &n, &k);
    assert(res == 2);

    abel::Matrix<unsigned long long> cur(MAX_HEIGHT, 1, 0);
    cur(0, 0) = 1;

    for (unsigned i = 0; i < n; ++i) {
        unsigned long long a = 0;
        unsigned long long b = 0;
        unsigned c = 0;

        res = scanf("%llu %llu %u", &a, &b, &c);
        assert(res == 3);

        b = std::min(b, k);

        cur = transitions[c].pow(std::min(b, k) - a, MOD).mul(cur, MOD);
    }

    printf("%llu\n", cur(0, 0));

    return 0;
}

/*

As usual, we rely on matrix power and multiplication. transitions holds legal transitions, cur[i] - the current number to reach (x, i)

*/
