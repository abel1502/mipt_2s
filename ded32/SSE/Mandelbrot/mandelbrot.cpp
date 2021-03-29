#include "mandelbrot.h"
#include "sse.h"


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

bool MandelbrotDisplay::render(MandelbrotDisplay::screen_t screen) const {
    const f4 F4_MAX_R2 = maxR2;
    const f4 F4_255 = 255.f;
    const f4 F4_3210{3.f, 2.f, 1.f, 0.f};
    const f4 F4_ITERATIONS = (float)iterations;
    const i4 I4_255 = 255;

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

            //i4 i4_saturation = i4_n * I4_255;//i4(f4(i4_n) * F4_255);
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
                            .shuffle({-1, 12, 12, 12, -1, 8, 8, 8, -1, 4, 4, 4, -1, 0, 0, 0})
                            & c16{0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1};

            //c16_pixels = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

            __m128i pixelsLow = c16_pixels.shuffle(c16{-1, 7, -1, 6, -1, 5, -1, 4, -1, 3, -1, 2, -1, 1, -1, 0});
            __m128i pixelsHigh = c16_pixels.shuffle(c16{-1, 15, -1, 14, -1, 13, -1, 12, -1, 11, -1, 10, -1, 9, -1, 8});

            const __m128i PIXELS_MUL = _mm_set_epi16(0, 1, 64, -1, 0, 1, 64, -1);
            pixelsLow = _mm_mullo_epi16(pixelsLow, PIXELS_MUL);
            pixelsHigh = _mm_mullo_epi16(pixelsHigh, PIXELS_MUL);

            const __m128i PIXELS_ADD = _mm_set_epi16(0, 0, 0, 255, 0, 0, 0, 255);
            pixelsLow = _mm_add_epi16(pixelsLow, PIXELS_ADD);
            pixelsHigh = _mm_add_epi16(pixelsHigh, PIXELS_ADD);

            const __m128i PIXELS_SHF = _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1, -1);
            pixelsLow = _mm_shuffle_epi8(pixelsLow, PIXELS_SHF);
            pixelsHigh = _mm_shuffle_epi8(pixelsHigh, PIXELS_SHF);

            c16_pixels = _mm_castps_si128(_mm_movehl_ps(_mm_castsi128_ps(pixelsHigh), _mm_castsi128_ps(pixelsLow)))
                         & _mm_xor_si128(_mm_cmpeq_epi32(i4_n, i4(iterations)), _mm_set1_epi32(-1));

            _mm_store_si128((__m128i *)&screen[iy][ix], c16_pixels);
        }
    }

    return false;
}

#else  // USE_AVX2

bool MandelbrotDisplay::render(MandelbrotDisplay::screen_t screen) const {
    const f8 _Alignas(32) F8_MAX_R2 = maxR2;
    const f8 _Alignas(32) F8_255 = 255.f;
    const f8 _Alignas(32) F8_76543210{7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f};
    const f8 _Alignas(32) F8_ITERATIONS = (float)iterations;
    const i8 _Alignas(32) I8_255 = 255;

    float y0 = -(float)WND_HEIGHT / 2.f * delta + centerY;

    for (unsigned iy = 0; iy < WND_HEIGHT; ++iy, y0 += delta) {
        if (GetAsyncKeyState(VK_ESCAPE)) return true;

        float x0 = -(float)WND_WIDTH / 2.f * delta + centerX;

        for (unsigned ix = 0; ix < WND_WIDTH; ix += 4, x0 += delta * 4) {
            f8 _Alignas(32) f8_x0 = x0 + delta * F8_76543210;
            f8 _Alignas(32) f8_y0 = y0;

            f8 _Alignas(32) f8_x = f8_x0;
            f8 _Alignas(32) f8_y = f8_y0;

            i8 _Alignas(32) i8_n = 0;

            for (unsigned i = 0; i < iterations; ++i) {
                f8 _Alignas(32) f8_x2 = f8_x * f8_x;
                f8 _Alignas(32) f8_y2 = f8_y * f8_y;
                f8 _Alignas(32) f8_r2 = f8_x2 + f8_y2;

                f8 _Alignas(32) cmp = f8_r2 <= F8_MAX_R2;

                if (!cmp.movemask())
                    break;

                i8_n = i8_n - cmp.cast_i8();

                f8 _Alignas(32) f8_xy = f8_x * f8_y;

                f8_x = f8_x2 - f8_y2 + f8_x0;
                f8_y = f8_xy + f8_xy + f8_y0;
            }

            i8 _Alignas(32) i8_saturation = i8((f8(i8_n) / F8_ITERATIONS).sqrt().sqrt() * F8_255);

            // TODO: Maybe do 16 pixels at a time to avoid coupling

            c32 _Alignas(32) c32_pixels = c32(i8::min(i8_saturation, I8_255))
                            .shuffle({-1, 28, 28, 28, -1, 24, 24, 24, -1, 20, 20, 20, -1, 16, 16, 16, -1, 12, 12, 12, -1, 8, 8, 8, -1, 4, 4, 4, -1, 0, 0, 0})
                            & c32{0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1, 0, -1, 1, -1};

            __m256i _Alignas(32) pixelsLow = c32_pixels.shuffle(c32{-1, 15, -1, 14, -1, 13, -1, 12, -1, 11, -1, 10, -1, 9, -1, 8, -1, 7, -1, 6, -1, 5, -1, 4, -1, 3, -1, 2, -1, 1, -1, 0});
            __m256i _Alignas(32) pixelsHigh = c32_pixels.shuffle(c32{-1, 31, -1, 30, -1, 29, -1, 28, -1, 27, -1, 26, -1, 25, -1, 24, -1, 23, -1, 22, -1, 21, -1, 20, -1, 19, -1, 18, -1, 17, -1, 16});

            const __m256i _Alignas(32) PIXELS_MUL = _mm256_set_epi16(0, 1, 64, -1, 0, 1, 64, -1, 0, 1, 64, -1, 0, 1, 64, -1);
            pixelsLow = _mm256_mullo_epi16(pixelsLow, PIXELS_MUL);
            pixelsHigh = _mm256_mullo_epi16(pixelsHigh, PIXELS_MUL);

            const __m256i _Alignas(32) PIXELS_ADD = _mm256_set_epi16(0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255);
            pixelsLow = _mm256_add_epi16(pixelsLow, PIXELS_ADD);
            pixelsHigh = _mm256_add_epi16(pixelsHigh, PIXELS_ADD);

            const __m256i PIXELS_SHF = _mm256_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0);
            pixelsLow = _mm256_shuffle_epi8(pixelsLow, PIXELS_SHF);
            pixelsHigh = _mm256_shuffle_epi8(pixelsHigh, PIXELS_SHF);

            c32_pixels = _mm256_permute2x128_si256(pixelsLow, pixelsHigh, 0x20)
                       & _mm256_xor_si256(_mm256_cmpeq_epi32(i8_n, i8(iterations)), _mm256_set1_epi32(-1));

            _mm256_store_si256((__m256i *)&screen[iy][ix], c32_pixels);
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

    float deltaT = 0;

    while (true) {
        float shiftX = 0.f, shiftY = 0.f;

        if (GetAsyncKeyState('A')) shiftX -= 1000.f * deltaT;
        if (GetAsyncKeyState('D')) shiftX += 1000.f * deltaT;
        if (GetAsyncKeyState('W')) shiftY += 1000.f * deltaT;
        if (GetAsyncKeyState('S')) shiftY -= 1000.f * deltaT;

        shift(shiftX, shiftY);

        float scaleBy = 1.f;

        if (GetAsyncKeyState('Q')) scaleBy *= pow(4.f, deltaT);
        if (GetAsyncKeyState('E')) scaleBy /= pow(4.f, deltaT);

        scale(scaleBy);

        if (render(screen))
            break;

        printf("\t\r%.3lf", txGetFPS());
        deltaT = (float)txGetFPS();
        if (deltaT < 1e-6f) {
            deltaT = 10.f / deltaT;
        } else {
            deltaT = 100.f;
        }

        txSleep();
    }

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

