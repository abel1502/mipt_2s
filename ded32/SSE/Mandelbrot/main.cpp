#include <TXLib.h>
#include <cassert>

#include "mandelbrot.h"


int main() {
    MandelbrotDisplay md{};

    md.renderLoop();

    return 0;

    //_controlfp(-1, 0x4 | 0x10);  // It took me so freaking much to figure out it was txlib that turned float overflows into exceptions instead of regular infinities...

    /*txCreateWindow(WND_WIDTH, WND_HEIGHT);
    Win32::_fpreset();
    txBegin();

    typedef RGBQUAD (&screen_t)[WND_HEIGHT][WND_WIDTH];

    screen_t screen = (screen_t)*txVideoMemory();

    const unsigned ITERATIONS = 256;
    const float DX = 1/800.f, DY = DX;
    const f4 F4_MAX_R2 = 100.f;
    const f4 F4_255 = 255.f;
    const f4 F4_3210{3.f, 2.f, 1.f, 0.f};
    const f4 F4_ITERATIONS = ITERATIONS;
    const i4 I4_255 = 255;


    while (true) {
        MAYBE_BREAK_();

        float y0 = -(float)WND_HEIGHT / 2.f * DY;

        for (unsigned iy = 0; iy < WND_HEIGHT; ++iy, y0 += DY) {
            MAYBE_BREAK_();

            float x0 = -(float)WND_WIDTH / 2.f * DX - 1.325f;

            for (unsigned ix = 0; ix < WND_WIDTH; ix += 4, x0 += DX * 4) {
                f4 f4_x0 = x0 + DX * F4_3210;
                f4 f4_y0 = y0;

                f4 f4_x = f4_x0;
                f4 f4_y = f4_y0;

                i4 i4_n = 0;

                //f4 cmp = f4_x <= _mm_setzero_ps();
                //i4_n = i4_n - cmp.cast_i4();

                for (unsigned i = 0; i < ITERATIONS; ++i) {
                    f4 f4_x2 = f4_x * f4_x;
                    f4 f4_y2 = f4_y * f4_y;
                    f4 f4_r2 = f4_x2 + f4_y2;

                    f4 cmp = f4_r2 <= F4_MAX_R2;

                    if (!cmp.movemask())
                        break;

                    i4_n = i4_n - cmp.cast_i4();

                    f4 f4_xy = f4_x * f4_y;

                    f4_x = f4_x2 - f4_y2 + f4_x0 + f4{1.f};
                    f4_y = f4_xy + f4_xy + f4_y0 + f4{1.f};
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

                pixelsLow = _mm_shuffle_epi8(pixelsLow, _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1, -1));
                pixelsHigh = _mm_shuffle_epi8(pixelsHigh, _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1, -1));

                c16_pixels = _mm_castps_si128(_mm_movehl_ps(_mm_castsi128_ps(pixelsHigh), _mm_castsi128_ps(pixelsLow)))
                             & _mm_xor_si128(_mm_cmpeq_epi32(i4_n, i4(ITERATIONS)), _mm_set1_epi32(-1));

                _mm_store_si128((__m128i *)&screen[iy][ix], c16_pixels);
            }
        }

        printf("\t\r%.3lf", txGetFPS());
        txSleep();
    }*/

    return 0;
}
