/*

јн€ подарила ¬ане последовательность, содержащую n целых чисел a1,\N{THIN SPACE}a2,\N{THIN SPACE}Е,\N{THIN SPACE}an. ќна знает, что он считает последовательность целых чисел x1,\N{THIN SPACE}x2,\N{THIN SPACE}Е,\N{THIN SPACE}xk великолепной, если дл€ всех j число xj Ч это одно из чисел a1,a2,Е,an, а также дл€ всех пар соседей их ксор (1\N{THIN SPACE}\N{LESS-THAN OR EQUAL TO}\N{THIN SPACE}i\N{THIN SPACE}\N{LESS-THAN OR EQUAL TO}\N{THIN SPACE}k\N{MINUS SIGN}1, xi \N{CIRCLED PLUS} xi\N{THIN SPACE}+\N{THIN SPACE}1) имеет в двоичном представлении количество единиц, кратное трЄм.

Ќайдите количество великолепных последовательностей длины k по модулю 109+7 в јнином подарке.

ќбратите внимание, если a=[1,1] и k=1, то ответ равен 2, поскольку вы должны рассматривать единицы из a как разные.

¬ходные данные
¬ первой строке находитс€ два целых числа n и k (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}100, 1\N{LESS-THAN OR EQUAL TO}k\N{LESS-THAN OR EQUAL TO}1018) Ч длина јниной последовательности и длина великолепной последовательности.

¬о второй строке наход€тс€ n целых чисел ai (0\N{LESS-THAN OR EQUAL TO}ai\N{LESS-THAN OR EQUAL TO}1018).

¬ыходные данные
¬ыведите количество великолепных последовательностей длины k по модулю 109+7.

ѕримеры
входные данные
5 2
15 1 2 4 8
выходные данные
13
входные данные
5 1
15 1 2 4 8
выходные данные
5

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
    unsigned long long k = 0;
    int res = scanf("%u %llu", &n, &k);
    assert(res == 2);

    std::vector<unsigned long long> a(n);
    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%llu", &a[i]);
        assert(res == 1);
    }

    abel::Matrix<unsigned long long> transition(n, n, 0);
    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j = i; j < n; ++j) {
            transition(i, j) = transition(j, i) = !(popcount(a[i] ^ a[j]) % 3);
        }
    }

    printf("%llu\n", transition.pow(k - 1, MOD).sum(MOD));

    return 0;
}

/*

Yet again, we raise a transition matrix to a power.

*/
