#ifndef SSE_H
#define SSE_H

#include <mmintrin.h>   // MMX
#include <xmmintrin.h>  // SSE
#include <emmintrin.h>  // SSE2
#include <pmmintrin.h>  // SSE3
#include <tmmintrin.h>  // SSSE3
#include <smmintrin.h>  // SSE4.1
#include <nmmintrin.h>  // SSE4.2
#include <immintrin.h>  // AVX, AVX2

#include <x86intrin.h>


namespace sse {


// No rule of 3 constructors, no const operator variants and none of such are implemented
// because these are intended as pretty much compile-time wrappers for sse types

// Warning: Type casts between i4 and f4 reinterpret and do not convert!

// TODO: ?u4 - 4*unsigned

// TODO: Cast operators: f4 <-> f8, i4 <-> i8, c16 <-> c32, i4 <-> c16, i8 <-> c32


struct i4;
struct f4;
struct c16;
struct i8;
struct f8;
struct c32;

//================================================================================

struct f4 {
    union {
        __m128 m;
        float f[4];
    };

    inline f4(__m128 new_m) :
        m{new_m} {}

    inline f4(float new_f) :
        m{_mm_set1_ps(new_f)} {}

    inline f4(float new_f1, float new_f2, float new_f3, float new_f4) :
        m{_mm_set_ps(new_f1, new_f2, new_f3, new_f4)} {}

    f4(const i4 &src);

    inline operator __m128() const {
        return m;
    }

    i4 cast_i4() const;

    operator i4() const;

    f8 cast_f8() const;

    explicit operator f8() const;

    inline float &operator[](size_t n) {
        return f[n];
    }

    inline int movemask() const {
        return _mm_movemask_ps(m);
    }

    inline friend f4 operator+(const f4 &a, const f4 &b) {
        return _mm_add_ps(a, b);
    }

    inline friend f4 operator-(const f4 &a, const f4 &b) {
        return _mm_sub_ps(a, b);
    }

    inline friend f4 operator*(const f4 &a, const f4 &b) {
        return _mm_mul_ps(a, b);
    }

    inline friend f4 operator/(const f4 &a, const f4 &b) {
        return _mm_div_ps(a, b);
    }

    inline f4 sqrt() const {
        return _mm_sqrt_ps(m);
    }

    inline friend f4 operator<(const f4 &a, const f4 &b) {
        return _mm_cmp_ps(a, b, _CMP_LT_OQ);
    }

    inline friend f4 operator<=(const f4 &a, const f4 &b) {
        return _mm_cmp_ps(a, b, _CMP_LE_OQ);
    }

    inline friend f4 operator>(const f4 &a, const f4 &b) {
        return _mm_cmp_ps(a, b, _CMP_GT_OQ);
    }

    inline friend f4 operator>=(const f4 &a, const f4 &b) {
        return _mm_cmp_ps(a, b, _CMP_GE_OQ);
    }

    inline friend f4 operator!=(const f4 &a, const f4 &b) {
        return _mm_cmp_ps(a, b, _CMP_NEQ_UQ);
    }

    inline friend f4 operator==(const f4 &a, const f4 &b) {
        return _mm_cmp_ps(a, b, _CMP_EQ_OQ);
    }
};

//--------------------------------------------------------------------------------

struct i4 {
    union {
        __m128i m;
        int i[4];
    };

    inline i4(__m128i new_m) :
        m{new_m} {}

    inline i4(int new_i) :
        m{_mm_set1_epi32(new_i)} {}

    inline i4(int new_i1, int new_i2, int new_i3, int new_i4) :
        m{_mm_set_epi32(new_i1, new_i2, new_i3, new_i4)} {}

    inline i4(const f4 &src) :
        m{_mm_cvtps_epi32(src)} {}

    inline operator __m128i() const {
        return m;
    }

    inline f4 cast_f4() const {
        return f4(_mm_castsi128_ps(m));
    }

    inline operator f4() const {
        return cast_f4();
    }

    i8 cast_i8() const;

    explicit operator i8() const;

    inline int &operator[](size_t n) {
        return i[n];
    }

    inline friend i4 operator+(const i4 &a, const i4 &b) {
        return _mm_add_epi32(a, b);
    }

    inline friend i4 operator-(const i4 &a, const i4 &b) {
        return _mm_sub_epi32(a, b);
    }

    inline friend i4 operator*(const i4 &a, const i4 &b) {
        return _mm_mullo_epi32(a, b);
    }

    static inline i4 min(const i4 &a, const i4 &b) {
        return _mm_min_epi32(a, b);
    }

    inline friend i4 operator&(const i4 &a, const i4 &b) {
        return _mm_and_si128(a, b);
    }
};

inline i4 f4::cast_i4() const {
    return i4(_mm_castps_si128(m));
}

inline f4::operator i4() const {
    return cast_i4();
}

inline f4::f4(const i4 &src) :
    m{_mm_cvtepi32_ps(src)} {}

//--------------------------------------------------------------------------------

struct c16 {
    union {
        __m128i m;
        char c[16];
    };

    inline c16(__m128i new_m) :
        m{new_m} {}

    inline c16(char new_c) :
        m{_mm_set1_epi8(new_c)} {}

    inline c16(char new_c1, char new_c2, char new_c3, char new_c4, char new_c5, char new_c6, char new_c7, char new_c8, char new_c9, char new_c10, char new_c11, char new_c12, char new_c13, char new_c14, char new_c15, char new_c16) :
        m{_mm_set_epi8(new_c1, new_c2, new_c3, new_c4, new_c5, new_c6, new_c7, new_c8, new_c9, new_c10, new_c11, new_c12, new_c13, new_c14, new_c15, new_c16)} {}

    inline c16(const i4 &src) :
        m{src.m} {};

    inline operator __m128i() const {
        return m;
    }

    inline i4 cast_i4() const {
        return i4(m);
    }

    inline operator i4() const {
        return cast_i4();
    }

    c32 cast_c32() const;

    explicit operator c32() const;

    inline char &operator[](size_t n) {
        return c[n];
    }

    inline friend c16 operator+(const c16 &a, const c16 &b) {
        return _mm_add_epi8(a, b);
    }

    inline friend c16 operator-(const c16 &a, const c16 &b) {
        return _mm_sub_epi8(a, b);
    }

    inline c16 shuffle(const c16 &mask) const {
        return _mm_shuffle_epi8(m, mask);
    }

    inline friend c16 operator&(const c16 &a, const c16 &b) {
        return _mm_and_si128(a, b);
    }
};

//================================================================================

struct f8 {
    union {
        __m256 m;
        float f[8];
    };

    inline f8(__m256 new_m) :
        m{new_m} {}

    inline f8(float new_f) :
        m{_mm256_set1_ps(new_f)} {}

    inline f8(float new_f1, float new_f2, float new_f3, float new_f4, float new_f5, float new_f6, float new_f7, float new_f8) :
        m{_mm256_set_ps(new_f1, new_f2, new_f3, new_f4, new_f5, new_f6, new_f7, new_f8)} {}

    f8(const i8 &src);

    inline operator __m256() const {
        return m;
    }

    i8 cast_i8() const;

    operator i8() const;

    inline f4 cast_f4() const {
        return _mm256_castps256_ps128(m);
    }

    inline explicit operator f4() const {
        return cast_f4();
    }

    inline float &operator[](size_t n) {
        return f[n];
    }

    inline int movemask() const {
        return _mm256_movemask_ps(m);
    }

    inline friend f8 operator+(const f8 &a, const f8 &b) {
        return _mm256_add_ps(a, b);
    }

    inline friend f8 operator-(const f8 &a, const f8 &b) {
        return _mm256_sub_ps(a, b);
    }

    inline friend f8 operator*(const f8 &a, const f8 &b) {
        return _mm256_mul_ps(a, b);
    }

    inline friend f8 operator/(const f8 &a, const f8 &b) {
        return _mm256_div_ps(a, b);
    }

    inline f8 sqrt() const {
        return _mm256_sqrt_ps(m);
    }

    inline friend f8 operator<(const f8 &a, const f8 &b) {
        return _mm256_cmp_ps(a, b, _CMP_LT_OQ);
    }

    inline friend f8 operator<=(const f8 &a, const f8 &b) {
        return _mm256_cmp_ps(a, b, _CMP_LE_OQ);
    }

    inline friend f8 operator>(const f8 &a, const f8 &b) {
        return _mm256_cmp_ps(a, b, _CMP_GT_OQ);
    }

    inline friend f8 operator>=(const f8 &a, const f8 &b) {
        return _mm256_cmp_ps(a, b, _CMP_GE_OQ);
    }

    inline friend f8 operator!=(const f8 &a, const f8 &b) {
        return _mm256_cmp_ps(a, b, _CMP_NEQ_UQ);
    }

    inline friend f8 operator==(const f8 &a, const f8 &b) {
        return _mm256_cmp_ps(a, b, _CMP_EQ_OQ);
    }
};

inline f8 f4::cast_f8() const {
    return _mm256_castps128_ps256(m);
}

inline f4::operator f8() const {
    return cast_f8();
}

//--------------------------------------------------------------------------------

struct i8 {
    union {
        __m256i m;
        int i[8];
    };

    inline i8(__m256i new_m) :
        m{new_m} {}

    inline i8(int new_i) :
        m{_mm256_set1_epi32(new_i)} {}

    inline i8(int new_i1, int new_i2, int new_i3, int new_i4, int new_i5, int new_i6, int new_i7, int new_i8) :
        m{_mm256_set_epi32(new_i1, new_i2, new_i3, new_i4, new_i5, new_i6, new_i7, new_i8)} {}

    inline i8(const f8 &src) :
        m{_mm256_cvtps_epi32(src)} {}

    inline operator __m256i() const {
        return m;
    }

    inline f8 cast_f8() const {
        return f8(_mm256_castsi256_ps(m));
    }

    inline operator f8() const {
        return cast_f8();
    }

    inline i4 cast_i4() const {
        return _mm256_castsi256_si128(m);
    }

    inline explicit operator i4() const {
        return cast_i4();
    }

    inline int &operator[](size_t n) {
        return i[n];
    }

    inline friend i8 operator+(const i8 &a, const i8 &b) {
        return _mm256_add_epi32(a, b);
    }

    inline friend i8 operator-(const i8 &a, const i8 &b) {
        return _mm256_sub_epi32(a, b);
    }

    inline friend i8 operator*(const i8 &a, const i8 &b) {
        return _mm256_mul_epi32(a, b);
    }

    static inline i8 min(const i8 &a, const i8 &b) {
        return _mm256_min_epi32(a, b);
    }

    inline friend i8 operator&(const i8 &a, const i8 &b) {
        return _mm256_and_si256(a, b);
    }
};


inline i8 f8::cast_i8() const {
    return i8(_mm256_castps_si256(m));
}

inline f8::operator i8() const {
    return cast_i8();
}

inline f8::f8(const i8 &src) :
    m{_mm256_cvtepi32_ps(src)} {}

inline i8 i4::cast_i8() const {
    return _mm256_castsi128_si256(m);
}

inline i4::operator i8() const {
    return cast_i8();
}

//--------------------------------------------------------------------------------

struct c32 {
    union {
        __m256i m;
        char c[32];
    };

    inline c32(__m256i new_m) :
        m{new_m} {}

    inline c32(char new_c) :
        m{_mm256_set1_epi8(new_c)} {}

    inline c32(char new_c1, char new_c2, char new_c3, char new_c4, char new_c5, char new_c6, char new_c7, char new_c8, char new_c9, char new_c10, char new_c11, char new_c12, char new_c13, char new_c14, char new_c15, char new_c16, char new_c17, char new_c18, char new_c19, char new_c20, char new_c21, char new_c22, char new_c23, char new_c24, char new_c25, char new_c26, char new_c27, char new_c28, char new_c29, char new_c30, char new_c31, char new_c32) :
        m{_mm256_set_epi8(new_c1, new_c2, new_c3, new_c4, new_c5, new_c6, new_c7, new_c8, new_c9, new_c10, new_c11, new_c12, new_c13, new_c14, new_c15, new_c16, new_c17, new_c18, new_c19, new_c20, new_c21, new_c22, new_c23, new_c24, new_c25, new_c26, new_c27, new_c28, new_c29, new_c30, new_c31, new_c32)} {}

    inline c32(const i8 &src) :
        m{src.m} {};

    inline operator __m256i() const {
        return m;
    }

    inline i8 cast_i8() const {
        return i8(m);
    }

    inline operator i8() const {
        return cast_i8();
    }

    inline c16 cast_c16() const {
        return c16(_mm256_castsi256_si128(m));
    }

    inline explicit operator c16() const {
        return cast_c16();
    }

    inline char &operator[](size_t n) {
        return c[n];
    }

    inline friend c32 operator+(const c32 &a, const c32 &b) {
        return _mm256_add_epi8(a, b);
    }

    inline friend c32 operator-(const c32 &a, const c32 &b) {
        return _mm256_sub_epi8(a, b);
    }

    inline c32 shuffle(const c32 &mask) const {
        return _mm256_shuffle_epi8(m, mask);
    }

    inline friend c32 operator&(const c32 &a, const c32 &b) {
        return _mm256_and_si256(a, b);
    }
};

inline c32 c16::cast_c32() const {
    return c32(_mm256_castsi128_si256(m));
}

inline c16::operator c32() const {
    return cast_c32();
}



}


#endif // SSE_H


