
#include "TXLib.h"
#include <emmintrin.h>

//=================================================================================================

const float ROI_X = -1.325f,
            ROI_Y = 0;

//=================================================================================================

union mmxi_t;

union mmxf_t
    {
    __m128 m;
    float  f[4];

    mmxf_t (__m128 val);
    mmxf_t (float val);
    mmxf_t (float val1, float val2, float val3, float val4);
    mmxf_t (const mmxi_t& val);

    operator __m128() const;
    operator mmxi_t() const;

    float& operator[] (size_t n);
    };

//-------------------------------------------------------------------------------------------------

union mmxi_t
    {
    __m128i m;
    int     i[4];

    mmxi_t (__m128i val);
    mmxi_t (int val);
    mmxi_t (int val1, int val2, int val3, int val4);

    operator __m128i() const;

    int& operator[] (size_t n);
    };

//-------------------------------------------------------------------------------------------------

inline mmxf_t::mmxf_t (__m128 val)                                      : m (val)                                      {}
inline mmxf_t::mmxf_t (float  val)                                      : m (_mm_set_ps1     (val))                    {}
inline mmxf_t::mmxf_t (float  val1, float val2, float val3, float val4) : m (_mm_set_ps      (val1, val2, val3, val4)) {}
inline mmxf_t::mmxf_t (const mmxi_t& val)                               : m (_mm_cvtepi32_ps (val))                    {}

inline mmxf_t::operator __m128() const                                  { return m;                                     }
inline mmxf_t::operator mmxi_t() const                                  { return mmxi_t (_mm_castps_si128 (m));         }
inline float& mmxf_t::operator[] (size_t n)                             { return f[n];                                  }

//-------------------------------------------------------------------------------------------------

inline mmxi_t::mmxi_t (__m128i val)                                     : m (val)                                      {}
inline mmxi_t::mmxi_t (int     val)                                     : m (_mm_set1_epi32  (val))                    {}
inline mmxi_t::mmxi_t (int     val1, int val2, int val3, int val4)      : m (_mm_set_epi32   (val1, val2, val3, val4)) {}

inline mmxi_t::operator __m128i() const                                 { return m;                                     }
inline int& mmxi_t::operator[] (size_t n)                               { return i[n];                                  }

//-------------------------------------------------------------------------------------------------

inline mmxf_t operator +  (const mmxf_t& a, const mmxf_t& b) { return _mm_add_ps      (a, b); }
inline mmxf_t operator -  (const mmxf_t& a, const mmxf_t& b) { return _mm_sub_ps      (a, b); }
inline mmxf_t operator *  (const mmxf_t& a, const mmxf_t& b) { return _mm_mul_ps      (a, b); }
inline mmxf_t operator /  (const mmxf_t& a, const mmxf_t& b) { return _mm_div_ps      (a, b); }
inline mmxf_t operator <= (const mmxf_t& a, const mmxf_t& b) { return _mm_cmple_ps    (a, b); }
inline int    operator +  (const mmxf_t& cmp)                { return _mm_movemask_ps (cmp);  }

inline mmxf_t sqrt        (const mmxf_t& x)                  { return _mm_sqrt_ps     (x);    }

inline mmxi_t operator +  (const mmxi_t& a, const mmxi_t& b) { return _mm_add_epi32   (a, b); }
inline mmxi_t operator -  (const mmxi_t& a, const mmxi_t& b) { return _mm_sub_epi32   (a, b); }

//-------------------------------------------------------------------------------------------------

int main()
    {
    txCreateWindow (800, 600);
    Win32::_fpreset();
    txBegin();

    unsigned currentControl = 0;
    _controlfp_s(&currentControl, 0, 0);
    printf("%x\n", currentControl);

    typedef RGBQUAD (&scr_t) [600][800];
    scr_t scr = (scr_t) *txVideoMemory();

    const int    nMax  = 256;
    const float  dx    = 1/800.f, dy = 1/800.f;
    const mmxf_t r2Max = 100.f;
    const mmxf_t _255  = 255.f;
    const mmxf_t _3210 (3.f, 2.f, 1.f, 0.f);

    const mmxf_t nmax  = nMax;

    float xC = 0.f, yC = 0.f, scale = 1.f;

    for (;;)
        {
        if (GetAsyncKeyState (VK_ESCAPE)) break;

        if (txGetAsyncKeyState (VK_RIGHT)) xC    += dx * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        if (txGetAsyncKeyState (VK_LEFT))  xC    -= dx * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        if (txGetAsyncKeyState (VK_DOWN))  yC    += dy * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        if (txGetAsyncKeyState (VK_UP))    yC    -= dy * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        if (txGetAsyncKeyState ('A'))      scale += dx * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        if (txGetAsyncKeyState ('Z'))      scale -= dx * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);

        for (int iy = 0; iy < 600; iy++)
            {
            if (GetAsyncKeyState (VK_ESCAPE)) break;

            float x0 = ( (          - 400.f) * dx + ROI_X + xC ) * scale,
                  y0 = ( ((float)iy - 300.f) * dy + ROI_Y + yC ) * scale;

            for (int ix = 0; ix < 800; ix += 4, x0 += dx*4)
                {
                mmxf_t X0 = x0 + dx*_3210;
                mmxf_t Y0 = y0;

                mmxf_t X = X0, Y = Y0;
                mmxi_t N = 0;

                for (int n = 0; n < nMax; n++)
                    {
                    mmxf_t x2 = X*X, y2 = Y*Y;
                    mmxf_t r2 = x2 + y2;

                    mmxf_t cmp = (r2 <= r2Max);
                    if (! +cmp) break;

                    N = N - (mmxi_t) cmp;

                    mmxf_t xy = X*Y;

                    X = x2 - y2 + X0;
                    Y = xy + xy + Y0;
                    }

                mmxf_t I = sqrt (sqrt (N/nmax)) * _255;

                for (int i = 0; i < 4; i++)
                    {
                    BYTE c = (BYTE) I[i];
                    RGBQUAD color = (N[i] < nMax)? RGBQUAD { (BYTE) (255-c), (BYTE) (c%2 * 64), c } : RGBQUAD {};
                    scr[iy][ix+i] = color;
                    }
                }
            }

        printf ("\t\r%.0lf", txGetFPS());
        txSleep();
        }
    }



