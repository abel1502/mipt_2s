/*

Гномы прорыли очень удобную для них систему тоннелей в пещерах. У неё есть только один вход и один выход. Равноудаленные от входа по количеству пройденных тоннелей пещеры на языке гномов называется обскварк.

В системе тоннелей есть L обскварков и N пещер на каждом из них. Рассмотрим два соседних обскварка L1 и L2. Чтобы можно было быстро передвигаться в тоннелях, каждая пещера из уровня L1 соединена с каждой пещерой из уровня L2. Для перехода из i-й перещы первого обскварка в j-ю пещеру второго (i,j\N{ELEMENT OF}{1, 2, …, N}) необходимо cij миллилитров масла в фонаре. Для каждой пары соседних обскварков это количество одинаковое. Также количество масла на переход в каждую пещеру L2 тоже одинаковое для всех пещер из L1, то есть cij одинаково для i\N{ELEMENT OF}{1, 2, …, N} и фиксированного j.

У Гимли есть бутылки для масла емкостью M миллилитров. На свой путь по таким пещерам он хочет взять с собой несколько полных бутылочек с маслом. Гимли просит вас помочь ему и найти количество путей между точками входа и выхода таких, что суммарное количество масла будет делиться на заданное число M.

Входные данные
Первая строка ввода содержит N (1\N{LESS-THAN OR EQUAL TO}N\N{LESS-THAN OR EQUAL TO}106) –– число пещер в обскварке, L (2\N{LESS-THAN OR EQUAL TO}L\N{LESS-THAN OR EQUAL TO}105) – число обскварков, и M (2\N{LESS-THAN OR EQUAL TO}M\N{LESS-THAN OR EQUAL TO}100).

Вторая, третья и четвёртые строки содержат N целых чисел, 0\N{LESS-THAN OR EQUAL TO}cost\N{LESS-THAN OR EQUAL TO}M обозначающих количество масла, необходимое для перехода от точки входа до первого обскварка, количество масла, необходимое для перехода между соседними обскварками, как описано выше, и количество масла, необходимое для перехода от последнего обскварка до точки выхода.

Выходные данные
Выведите число путей по модулю 109+7, которые может выбрать Гимли так, чтобы суммарное количество масла на каждом пути делилось бы на M.

Пример
входные данные
2 3 13
4 6
2 1
3 4
выходные данные
2
Примечание


Это система тоннелей с 3 обскварками, на каждом обскварке по 2 пещеры. Пути 6\N{RIGHTWARDS ARROW}2\N{RIGHTWARDS ARROW}2\N{RIGHTWARDS ARROW}3, и 6\N{RIGHTWARDS ARROW}2\N{RIGHTWARDS ARROW}1\N{RIGHTWARDS ARROW}4 - единственные с суммарным количеством масла, делящимся на 13. Обратите внимание, что все входящие в пещеру тоннели имеют одинковое количество масла на переход и то, что они одинаковы для всех уровней.

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


unsigned popcount(unsigned long long x) {
    unsigned result = 0;

    while (x) {
        result += x & 1;
        x >>= 1;
    }

    return result;
}


int main() {
    const unsigned long long MOD = 1000000007;

    unsigned n = 0;
    unsigned l = 0;
    unsigned m = 0;
    int res = scanf("%u %u %u", &n, &l, &m);
    assert(res == 3);

    std::vector<unsigned> in(n, 0), trans(n, 0), out(n, 0);
    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%u", &in[i]);
        assert(res == 1);
    }
    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%u", &trans[i]);
        assert(res == 1);
    }
    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%u", &out[i]);
        assert(res == 1);
    }

    abel::Matrix<unsigned long long> entrance(m, 1, 0);
    for (unsigned i = 0; i < n; ++i) {
        entrance(in[i] % m, 0) += 1;
    }

    abel::Matrix<unsigned long long> dp(m, m, 0);
    for (unsigned i = 0; i < n; ++i) {
        dp(0, (m - trans[i]) % m) += 1;
    }
    for (unsigned i = 1; i < m; ++i) {
        for (unsigned j = 0; j < m; ++j) {
            dp(i, j) = dp(i - 1, (j + m - 1) % m);
        }
    }

    abel::Matrix<unsigned long long> exit(1, m, 0);
    for (unsigned i = 0; i < n; ++i) {
        exit(0, (2 * m - trans[i] - out[i]) % m) += 1;
    }

    printf("%llu\n", exit.mul(dp.pow(l - 2, MOD).mul(entrance, MOD), MOD).sum(MOD));

    return 0;
}

/*

More matrix manipulations. Entrance, dp and exit are various transition matrices. I think it's pretty apparent at this point

*/

