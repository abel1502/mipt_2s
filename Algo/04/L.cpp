/*

Компания BrokenTiles планирует заняться выкладыванием во дворах у состоятельных клиентов узор из черных и белых плиток, каждая из которых имеет размер 1\N{MULTIPLICATION SIGN}1 метр. Известно, что дворы всех состоятельных людей имеют наиболее модную на сегодня форму прямоугольника M\N{MULTIPLICATION SIGN}N метров.

Однако при составлении финансового плана у директора этой организации появилось целых две серьезных проблемы: во первых, каждый новый клиент очевидно захочет, чтобы узор, выложенный у него во дворе, отличался от узоров всех остальных клиентов этой фирмы, а во вторых, этот узор должен быть симпатичным.

Как показало исследование, узор является симпатичным, если в нем нигде не встречается квадрата 2\N{MULTIPLICATION SIGN}2 метра, полностью покрытого плитками одного цвета.

Для составления финансового плана директору необходимо узнать, сколько клиентов он сможет обслужить, прежде чем симпатичные узоры данного размера закончатся. Помогите ему!

Входные данные
На первой строке входного файла находятся три натуральных числа n, m, mod. 1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}10100, 1\N{LESS-THAN OR EQUAL TO}m\N{LESS-THAN OR EQUAL TO}6, 1\N{LESS-THAN OR EQUAL TO}mod\N{LESS-THAN OR EQUAL TO}10000.

n, m — размеры двора. mod — модуль, по которому нужно посчитать ответ.

Выходные данные
Выведите в выходной файл единственное число — количество различных симпатичных узоров, которые можно выложить во дворе размера n\N{MULTIPLICATION SIGN}m по модулю mod. Узоры, получающиеся друг из друга сдвигом, поворотом или отражением считаются различными.

Примеры
входные данные
2 2 5
выходные данные
4
входные данные
3 3 23
выходные данные
0

*/

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <string>
#include <algorithm>
#include <vector>
#include <numeric>


namespace abel {


class BigInt {
public:
    using digit_t = unsigned;  // Can't be unsigned long long for multiplication to work correctly

    static const digit_t DIGIT_MASK = (digit_t)-1;
    static const unsigned DIGIT_BITS = sizeof(digit_t) << 3;

    BigInt() : buf{} {}

    template <typename T>
    BigInt(T integer) :
        BigInt() {

        static_assert(std::is_arithmetic<T>::value);

        if (sizeof(T) << 3 <= DIGIT_BITS) {
            buf.push_back(integer);
            return;
        }

        while (integer) {
            buf.push_back(integer & DIGIT_MASK);
            integer >>= DIGIT_BITS;
        }
    }

    BigInt(const BigInt &other) :
        buf(other.buf) {}

    BigInt &operator=(const BigInt &other) {
        buf = other.buf;

        return *this;
    }

    template <typename T>
    BigInt &operator=(T integer) {
        static_assert(std::is_arithmetic<T>::value);

        return *this = BigInt(integer);
    }

    ~BigInt() {}

    template <typename T>
    T toInteger() const {
        T result = 0;

        for (const digit_t &digit : buf) {
            result <<= DIGIT_BITS;
            result |= digit;
        }

        return result;
    }

    friend bool operator==(const BigInt &bigNum, unsigned long long num) {
        return bigNum.buf.size() <= (sizeof(unsigned long long) / sizeof(digit_t)) && \
               bigNum.toInteger<unsigned long long>() == num;
    }

    friend bool operator==(unsigned long long num, const BigInt &bigNum) {
        return bigNum == num;
    }

    friend bool operator!=(const BigInt &bigNum, unsigned long long num) {
        return !(bigNum == num);
    }

    friend bool operator!=(unsigned long long num, const BigInt &bigNum) {
        return !(bigNum == num);
    }

    BigInt &operator++() {
        digit_t carry = 1;

        for (unsigned i = 0; i < buf.size() && carry; ++i) {
            buf[i] += carry;
            carry = !buf[i];
        }

        if (carry)
            buf.push_back(carry);

        return *this;
    }

    BigInt &operator--() {
        digit_t carry = 1;

        for (unsigned i = 0; i < buf.size() && carry; ++i) {
            buf[i] -= carry;
            carry = !~buf[i];
        }

        if (carry)
            assert(false);  // Negative numbers not supported

        if (!buf.empty() && !buf.back())
            buf.pop_back();

        return *this;
    }

    BigInt &operator+=(unsigned long long other) {
        digit_t carry = 0;

        for (unsigned i = 0; i < buf.size() && (carry || other); ++i) {
            digit_t was = buf[i];

            buf[i] += (other & DIGIT_MASK) + carry;
            other >>= DIGIT_BITS;

            carry = buf[i] < was || (buf[i] == was && carry);
        }

        if (carry)
            buf.push_back(carry);

        return *this;
    }

    BigInt &operator-=(unsigned long long other) {
        digit_t carry = 0;

        for (unsigned i = 0; i < buf.size() && (carry || other); ++i) {
            digit_t was = buf[i];

            buf[i] -= (other & DIGIT_MASK) + carry;
            other >>= DIGIT_BITS;

            carry = buf[i] > was || (buf[i] == was && carry);
        }

        if (carry)
            assert(false);  // Negative numbers not supported

        while (!buf.empty() && !buf.back())
            buf.pop_back();

        return *this;
    }

    friend BigInt operator+(BigInt bigNum, unsigned long long num) {
        return bigNum += num;
    }

    friend BigInt operator+(unsigned long long num, BigInt bigNum) {
        return bigNum += num;
    }

    friend BigInt operator-(BigInt bigNum, unsigned long long num) {
        return bigNum -= num;
    }

    BigInt &operator+=(const BigInt &other) {
        digit_t carry = 0;

        while (buf.size() < other.buf.size()) {
            buf.push_back(0);
        }

        for (unsigned i = 0; i < buf.size() && (carry || i < other.buf.size()); ++i) {
            digit_t was = buf[i];
            digit_t addend = other.buf[i];

            buf[i] += addend + carry;

            //printf("Addition of digits #%d: %x + %x + %x -> %x + ", i, was, addend, carry, buf[i]);

            carry = buf[i] < was || (buf[i] == was && carry);

            //printf("%x * BASE\n", carry);
        }

        if (carry)
            buf.push_back(carry);

        return *this;
    }

    friend BigInt operator+(BigInt bigOne, const BigInt &bigTwo) {
        return bigOne += bigTwo;
    }

    BigInt &operator>>=(unsigned bits) {
        unsigned offset = bits / DIGIT_BITS;
        bits %= DIGIT_BITS;

        buf.push_back(0);

        if (bits != 0) {
            for (unsigned i = 0; i + offset + 1 < buf.size(); ++i) {
                buf[i] = ((buf[i + offset + 1] & ((1 << bits) - 1)) << (DIGIT_BITS - bits)) | (buf[i + offset] >> bits);
            }
        } else {
            for (unsigned i = 0; i + offset + 1 < buf.size(); ++i) {
                buf[i] = buf[i + offset];
            }
        }

        for (++offset; offset; --offset) {
            buf.pop_back();
        }

        while (!buf.empty() && !buf.back()) {
            buf.pop_back();
        }

        return *this;
    }

    friend BigInt operator>>(BigInt bigNum, unsigned bits) {
        return bigNum >>= bits;
    }

    BigInt &operator<<=(unsigned bits) {
        unsigned offset = bits / DIGIT_BITS;
        bits %= DIGIT_BITS;

        for (unsigned i = 0; i < offset + 1; ++i) {
            buf.push_back(0);
        }

        if (bits != 0) {
            for (unsigned i = buf.size() - 1; i > offset; --i) {
                buf[i] = (buf[i - offset] << bits) | (buf[i - offset - 1] >> (DIGIT_BITS - bits));
            }
        } else {
            for (unsigned i = buf.size() - 1; i > offset; --i) {
                buf[i] = buf[i - offset];
            }
        }

        buf[offset] = buf[0] << bits;

        for (; offset; --offset) {
            buf[offset - 1] = 0;
        }

        while (!buf.empty() && !buf.back()) {
            buf.pop_back();
        }

        return *this;
    }

    friend BigInt operator<<(BigInt bigNum, unsigned bits) {
        return bigNum <<= bits;
    }

    friend BigInt operator*(const BigInt &bigNum, digit_t num) {
        BigInt result = 0;

        static_assert(sizeof(digit_t) * 2 <= sizeof(unsigned long long));

        for (unsigned i = 0; i < bigNum.buf.size(); ++i) {
            unsigned long long product = (unsigned long long)num * (unsigned long long)bigNum.buf[i];

            //printf(">> "); (result).printHex();
            //printf(" + "); (BigInt(product) << (i * DIGIT_BITS)).printHex();

            result += BigInt(product) << (i * DIGIT_BITS);

            //printf(" = "); (result).printHex();
        }

        return result;
    }

    BigInt &operator*=(digit_t num) {
        return *this = *this * num;
    }

    bool isEven() const {
        return buf.empty() || buf.front() % 2 == 0;
    }

    void scan() {
        buf.resize(0);

        int digit = 0;

        while (isspace(digit = getc(stdin)));

        while (digit != EOF && !isspace(digit)) {
            *this *= 10;
            *this += digit - '0';

            digit = getc(stdin);
        }
    }

    void printHex() const {
        if (buf.empty()) {
            printf("0");
        } else {
            printf("%x", buf.back());
        }

        for (unsigned i = buf.size() - 2; i < (digit_t)-2; --i) {
            printf("%08x", buf[i]);
        }

        printf("\n");
    }

private:
    std::vector<digit_t> buf;
};


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
                result.buf[i][j] = buf[j][i];
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

    Matrix<T> pow(const BigInt &power, unsigned long long mod = 0) const {
        assert(isSqr());

        if (power == 0)
            return E(height);

        if (power == 1)
            return *this;

        if (power.isEven()) {
            Matrix<T> result = pow(power >> 1, mod);
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

    void print() {
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
    abel::BigInt n;
    unsigned m = 0;
    unsigned mod = 0;

    n.scan();

    int res = scanf("%u %u", &m, &mod);
    assert(res == 2);

    abel::Matrix<unsigned long long> transition(1 << m, 1 << m, 1);
    for (unsigned mask1 = 0; mask1 < (unsigned)1 << m; ++mask1) {
        for (unsigned mask2 = 0; mask2 < (unsigned)1 << m; ++mask2) {
            for (unsigned k = 0; k < m - 1; ++k) {
                unsigned sqrState = ((mask1 >> k) & 0b11) << 2 | ((mask2 >> k) & 0b11);

                if (sqrState == 0b0000 || sqrState == 0b1111) {
                    transition(mask1, mask2) = 0;
                    break;
                }
            }
        }
    }

    printf("%llu\n", transition.pow(--n, mod).sum(mod));

    return 0;
}

/*

Same as in K, but through matrix operations

*/



