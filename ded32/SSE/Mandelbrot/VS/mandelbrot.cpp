#include "mandelbrot.h"
#include "sse.h"

#include <chrono>


using namespace sse;


MandelbrotDisplay::MandelbrotDisplay() :
    iterations{256},
    delta{1.f / MandelbrotDisplay::WND_WIDTH},
    maxR2{100.f},
    centerX{-1.325f},
    centerY{0.f} {}

MandelbrotDisplay::MandelbrotDisplay(unsigned new_iterations, float new_delta, float maxR, float new_centerX, float new_centerY) :
    iterations{new_iterations},
    delta{new_delta},
    maxR2{maxR * maxR},
    centerX{new_centerX},
    centerY{new_centerY} {}

#ifndef USE_AVX2

inline void drawOnScreen(MandelbrotDisplay::screen_t screen, unsigned ix, unsigned iy, i4 i4_n, const unsigned iterations) {
    const f4 F4_ITERATIONS = (float)iterations;
    const f4 F4_255 = 255.f;
    const i4 I4_255 = 255;

    i4 i4_saturation = i4((f4(i4_n) / F4_ITERATIONS).sqrt().sqrt() * F4_255);

    // When i4_n[i] == ITERATIONS, I want to zero out the corresponding pixel.
    // Otherwise:
    //
    // +----+----+----+----+
    // | B  | G  | R  | A  |
    // +----+----+----+----+
    // | 0  | 1  | 2  | 3  |
    // +----+----+----+----+
    // | C  | 00 | 00 | 00 |
    // +----+----+----+----+
    // | C  | C  | C  | 00 |
    // +----+----+----+----+
    // | x  | x&1| x  | 00 |
    // +----+----+----+----+
    // | -x |x*64| x  | 00 |
    // +----+----+----+----+
    // |ff+x| x  | x  | 00 |
    // +----+----+----+----+

    c16 c16_pixels = c16(i4::min(i4_saturation, I4_255))
        .shuffle({ -1, 12, 12, 12, -1, 8, 8, 8, -1, 4, 4, 4, -1, 0, 0, 0 })
        & c16 {
        0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1
    };

    //c16_pixels = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    __m128i pixelsLow = c16_pixels.shuffle(c16{ -1, 7, -1, 6, -1, 5, -1, 4, -1, 3, -1, 2, -1, 1, -1, 0 });
    __m128i pixelsHigh = c16_pixels.shuffle(c16{ -1, 15, -1, 14, -1, 13, -1, 12, -1, 11, -1, 10, -1, 9, -1, 8 });

    const __m128i PIXELS_MUL = _mm_set_epi16(0, 1, 64, -1, 0, 1, 64, -1);
    pixelsLow = _mm_mullo_epi16(pixelsLow, PIXELS_MUL);
    pixelsHigh = _mm_mullo_epi16(pixelsHigh, PIXELS_MUL);

    const __m128i PIXELS_ADD = _mm_set_epi16(0, 0, 0, 255, 0, 0, 0, 255);
    pixelsLow = _mm_add_epi16(pixelsLow, PIXELS_ADD);
    pixelsHigh = _mm_add_epi16(pixelsHigh, PIXELS_ADD);

    const __m128i PIXELS_SHF = _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1, -1);
    pixelsLow = _mm_shuffle_epi8(pixelsLow, PIXELS_SHF);
    pixelsHigh = _mm_shuffle_epi8(pixelsHigh, PIXELS_SHF);

    c16_pixels = _mm_and_si128(_mm_castps_si128(_mm_movehl_ps(_mm_castsi128_ps(pixelsHigh), _mm_castsi128_ps(pixelsLow))),
        _mm_xor_si128(_mm_cmpeq_epi32(i4_n, i4(iterations)), _mm_set1_epi32(-1)));

    _mm_store_si128((__m128i*) & screen[iy][ix], c16_pixels);
}

bool MandelbrotDisplay::render(MandelbrotDisplay::screen_t screen, bool perfCnt) const {
    const f4 F4_MAX_R2 = maxR2;
    
    const f4 F4_3210{3.f, 2.f, 1.f, 0.f};

    float y0 = -(float)WND_HEIGHT / 2.f * delta + centerY;

    for (unsigned iy = 0; iy < WND_HEIGHT; ++iy, y0 += delta) {
        if (GetAsyncKeyState(VK_ESCAPE)) return true;

        float x0 = -(float)WND_WIDTH / 2.f * delta + centerX;

        for (unsigned ix = 0; ix < WND_WIDTH; ix += 4, x0 += delta * 4) {
            f4 f4_x0 = x0 + delta * F4_3210;
            f4 f4_y0 = y0;

            f4 f4_x = f4_x0;
            f4 f4_y = f4_y0;

            i4 i4_n = 0;

            for (unsigned i = 0; i < iterations; ++i) {
                f4 f4_x2 = f4_x * f4_x;
                f4 f4_y2 = f4_y * f4_y;
                f4 f4_r2 = f4_x2 + f4_y2;

                f4 cmp = f4_r2 <= F4_MAX_R2;

                if (!cmp.movemask())
                    break;

                i4_n = i4_n - cmp.cast_i4();

                f4 f4_xy = f4_x * f4_y;

                f4_x = f4_x2 - f4_y2 + f4_x0;
                f4_y = f4_xy + f4_xy + f4_y0;
            }

            if (!perfCnt)
                drawOnScreen(screen, ix, iy, i4_n, iterations);
        }
    }

    return false;
}

#else  // USE_AVX2

inline void drawOnScreen(MandelbrotDisplay::screen_t screen, unsigned ix, unsigned iy, i8 i8_n, const unsigned iterations) {
    const f8 F8_ITERATIONS = (float)iterations;
    const f8 F8_255 = 255.f;
    const i8 I8_255 = 255;
    
    i8 i8_saturation = i8((f8(i8_n) / F8_ITERATIONS).sqrt().sqrt() * F8_255);

#if 0
    c16 c16_pixels = c16(i4::min(_mm256_extracti128_si256(i8_saturation, 0), _mm256_extracti128_si256(I8_255, 0)))
        .shuffle({ -1, 12, 12, 12, -1, 8, 8, 8, -1, 4, 4, 4, -1, 0, 0, 0 })
        & c16 {
        0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1
    };

    __m128i pixelsLow = c16_pixels.shuffle(c16{ -1, 7, -1, 6, -1, 5, -1, 4, -1, 3, -1, 2, -1, 1, -1, 0 });
    __m128i pixelsHigh = c16_pixels.shuffle(c16{ -1, 15, -1, 14, -1, 13, -1, 12, -1, 11, -1, 10, -1, 9, -1, 8 });

    const __m128i PIXELS_MUL = _mm_set_epi16(0, 1, 64, -1, 0, 1, 64, -1);
    pixelsLow = _mm_mullo_epi16(pixelsLow, PIXELS_MUL);
    pixelsHigh = _mm_mullo_epi16(pixelsHigh, PIXELS_MUL);

    const __m128i PIXELS_ADD = _mm_set_epi16(0, 0, 0, 255, 0, 0, 0, 255);
    pixelsLow = _mm_add_epi16(pixelsLow, PIXELS_ADD);
    pixelsHigh = _mm_add_epi16(pixelsHigh, PIXELS_ADD);

    const __m128i PIXELS_SHF = _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1, -1);
    pixelsLow = _mm_shuffle_epi8(pixelsLow, PIXELS_SHF);
    pixelsHigh = _mm_shuffle_epi8(pixelsHigh, PIXELS_SHF);

    c16_pixels = _mm_and_si128(_mm_castps_si128(_mm_movehl_ps(_mm_castsi128_ps(pixelsHigh), _mm_castsi128_ps(pixelsLow))),
        _mm_xor_si128(_mm_cmpeq_epi32(_mm256_extracti128_si256(i8_n, 0), i4(iterations)), _mm_set1_epi32(-1)));

    _mm_store_si128((__m128i*) & screen[iy][ix], c16_pixels);

    c16_pixels = c16(i4::min(_mm256_extracti128_si256(i8_saturation, 1), _mm256_extracti128_si256(I8_255, 1)))
        .shuffle({ -1, 12, 12, 12, -1, 8, 8, 8, -1, 4, 4, 4, -1, 0, 0, 0 })
        & c16 {
        0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1
    };

    pixelsLow = c16_pixels.shuffle(c16{ -1, 7, -1, 6, -1, 5, -1, 4, -1, 3, -1, 2, -1, 1, -1, 0 });
    pixelsHigh = c16_pixels.shuffle(c16{ -1, 15, -1, 14, -1, 13, -1, 12, -1, 11, -1, 10, -1, 9, -1, 8 });

    pixelsLow = _mm_mullo_epi16(pixelsLow, PIXELS_MUL);
    pixelsHigh = _mm_mullo_epi16(pixelsHigh, PIXELS_MUL);

    pixelsLow = _mm_add_epi16(pixelsLow, PIXELS_ADD);
    pixelsHigh = _mm_add_epi16(pixelsHigh, PIXELS_ADD);

    pixelsLow = _mm_shuffle_epi8(pixelsLow, PIXELS_SHF);
    pixelsHigh = _mm_shuffle_epi8(pixelsHigh, PIXELS_SHF);

    c16_pixels = _mm_and_si128(_mm_castps_si128(_mm_movehl_ps(_mm_castsi128_ps(pixelsHigh), _mm_castsi128_ps(pixelsLow))),
        _mm_xor_si128(_mm_cmpeq_epi32(_mm256_extracti128_si256(i8_n, 1), i4(iterations)), _mm_set1_epi32(-1)));

    _mm_store_si128((__m128i*) & screen[iy][ix + 4], c16_pixels);

#else

    c32 c32_pixels = c32(i8::min(i8_saturation, I8_255))
        .shuffle({ -1, 28, 28, 28, -1, 24, 24, 24, -1, 20, 20, 20, -1, 16, 16, 16, -1, 12, 12, 12, -1, 8, 8, 8, -1, 4, 4, 4, -1, 0, 0, 0 })
        & c32 {
        0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1
    };

    //__m256i pixelsLow = c32_pixels.shuffle(c32{-1, 15, -1, 14, -1, 13, -1, 12, -1, 11, -1, 10, -1, 9, -1, 8, -1, 7, -1, 6, -1, 5, -1, 4, -1, 3, -1, 2, -1, 1, -1, 0});
    //__m256i pixelsHigh = c32_pixels.shuffle(c32{-1, 31, -1, 30, -1, 29, -1, 28, -1, 27, -1, 26, -1, 25, -1, 24, -1, 23, -1, 22, -1, 21, -1, 20, -1, 19, -1, 18, -1, 17, -1, 16});

    __m256i pixelsLow = _mm256_cvtepu8_epi16(c32_pixels.cast_c16());
    __m256i pixelsHigh = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(c32_pixels, 1));

    const __m256i PIXELS_MUL = _mm256_set_epi16(0, 1, 64, -1, 0, 1, 64, -1, 0, 1, 64, -1, 0, 1, 64, -1);
    pixelsLow = _mm256_mullo_epi16(pixelsLow, PIXELS_MUL);
    pixelsHigh = _mm256_mullo_epi16(pixelsHigh, PIXELS_MUL);

    const __m256i PIXELS_ADD = _mm256_set_epi16(0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255);
    pixelsLow = _mm256_add_epi16(pixelsLow, PIXELS_ADD);
    pixelsHigh = _mm256_add_epi16(pixelsHigh, PIXELS_ADD);

    const __m256i PIXELS_AND = _mm256_set1_epi16(255);
    pixelsLow = _mm256_and_si256(pixelsLow, PIXELS_AND);
    pixelsHigh = _mm256_and_si256(pixelsHigh, PIXELS_AND);

    c32_pixels = _mm256_and_si256(_mm256_permute4x64_epi64(_mm256_packus_epi16(pixelsLow, pixelsHigh), 0xd8),
        _mm256_xor_si256(_mm256_cmpeq_epi32(i8_n, i8(iterations)), _mm256_set1_epi32(-1)));

    _mm256_store_si256((__m256i*)&screen[iy][ix], c32_pixels);
#endif
}

bool MandelbrotDisplay::render(MandelbrotDisplay::screen_t screen, bool perfCnt) const {
    const f8 F8_MAX_R2 = maxR2;
    const f8 F8_76543210{7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f};

    float y0 = -(float)WND_HEIGHT / 2.f * delta + centerY;

    for (unsigned iy = 0; iy < WND_HEIGHT; ++iy, y0 += delta) {
        if (GetAsyncKeyState(VK_ESCAPE)) return true;

        float x0 = -(float)WND_WIDTH / 2.f * delta + centerX;

        for (unsigned ix = 0; ix < WND_WIDTH; ix += 8, x0 += delta * 8) {
            f8 f8_x0 = x0 + delta * F8_76543210;
            f8 f8_y0 = y0;

            f8 f8_x = f8_x0;
            f8 f8_y = f8_y0;

            i8 i8_n = 0;

            for (unsigned i = 0; i < iterations; ++i) {
                f8 f8_x2 = f8_x * f8_x;
                f8 f8_y2 = f8_y * f8_y;
                f8 f8_r2 = f8_x2 + f8_y2;

                f8 cmp = f8_r2 <= F8_MAX_R2;

                if (!cmp.movemask())
                    break;

                i8_n = i8_n - cmp.cast_i8();

                f8 f8_xy = f8_x * f8_y;

                f8_x = f8_x2 - f8_y2 + f8_x0;
                f8_y = f8_xy + f8_xy + f8_y0;
            }

            if (!perfCnt)
                drawOnScreen(screen, ix, iy, i8_n, iterations);
        }
    }

    return false;
}

#endif

void MandelbrotDisplay::renderLoop() {
    txCreateWindow(WND_WIDTH, WND_HEIGHT);
    Win32::_fpreset();
    txBegin();

    screen_t screen = (screen_t)*txVideoMemory();

    float deltaT = 0.f;

    while (true) {
        float shiftX = 0.f, shiftY = 0.f;

        if (GetAsyncKeyState('A') & 0x8001) shiftX -= 10000.f * deltaT;
        if (GetAsyncKeyState('D') & 0x8001) shiftX += 10000.f * deltaT;
        if (GetAsyncKeyState('W') & 0x8001) shiftY += 10000.f * deltaT;
        if (GetAsyncKeyState('S') & 0x8001) shiftY -= 10000.f * deltaT;

        shift(shiftX, shiftY);

        float scaleBy = 1.f;

        if (GetAsyncKeyState('Q') & 0x8001) scaleBy *= pow(64.f, deltaT);
        if (GetAsyncKeyState('E') & 0x8001) scaleBy /= pow(64.f, deltaT);

        scale(scaleBy);

        if (render(screen))
            break;

        printf("\t\r%.3lf", txGetFPS());
        deltaT = 1.f / (float)txGetFPS();
        deltaT = fmaxf(fminf(deltaT, 10.f), 0.001f);

        txSleep();
    }

    txEnd();
}

void MandelbrotDisplay::perfCount() const {
    constexpr unsigned TEST_CNT = 500;

    Win32::_fpreset();

    screen_t screen = (screen_t)*txVideoMemory();

    std::chrono::high_resolution_clock clk{};

    auto tStart = clk.now();

    for (unsigned i = 0; i < TEST_CNT; ++i) {
        render(screen, true);
    }

    auto tEnd = clk.now();

    printf("On average %lf seconds per render\n", (double)(tEnd - tStart).count() / 1e9f / TEST_CNT);

    txEnd();
}

void MandelbrotDisplay::shift(float distX, float distY) {
    const float COEFF = 1.f;

    centerX += distX * delta * COEFF;
    centerY += distY * delta * COEFF;
}

void MandelbrotDisplay::scale(float coeff) {
    delta *= coeff;
}

void MandelbrotDisplay::setRadius(float maxR) {
    maxR2 = maxR * maxR;
}

void MandelbrotDisplay::setIterations(unsigned new_iterations) {
    iterations = new_iterations;
}

void MandelbrotDisplay::reset() {
    *this = MandelbrotDisplay();
}

