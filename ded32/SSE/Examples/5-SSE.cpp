
#include "TXLib.h"
#include <emmintrin.h>

//=================================================================================================

const float ROI_X = -1.325f,
            ROI_Y = 0;

//=================================================================================================

int main()
    {
    txCreateWindow (800, 600);
    Win32::_fpreset();
    txBegin();

    typedef RGBQUAD (&scr_t) [600][800];
    scr_t scr = (scr_t) *txVideoMemory();
    
    const int    nMax  = 256;
    const float  dx    = 1/800.f, dy = 1/800.f;
    const __m128 r2Max = _mm_set_ps1 (100.f);
    const __m128 _255  = _mm_set_ps1 (255.f);
    const __m128 _3210 = _mm_set_ps  (3.f, 2.f, 1.f, 0.f);
    
    const __m128 nmax  = _mm_set_ps1 (nMax);

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
                __m128 X0 = _mm_add_ps (_mm_set_ps1 (x0), _mm_mul_ps (_3210, _mm_set_ps1 (dx)));
                __m128 Y0 =             _mm_set_ps1 (y0);

                __m128 X = X0, Y = Y0;
                
                __m128i N = _mm_setzero_si128();

                for (int n = 0; n < nMax; n++)
                    {
                    __m128 x2 = _mm_mul_ps (X, X),
                           y2 = _mm_mul_ps (Y, Y);
                           
                    __m128 r2 = _mm_add_ps (x2, y2);

                    __m128 cmp = _mm_cmple_ps (r2, r2Max);
                    int mask   = _mm_movemask_ps (cmp);
                    if (!mask) break;

                    N = _mm_sub_epi32 (N, _mm_castps_si128 (cmp));

                    __m128 xy = _mm_mul_ps (X, Y);

                    X = _mm_add_ps (_mm_sub_ps (x2, y2), X0);
                    Y = _mm_add_ps (_mm_add_ps (xy, xy), Y0);
                    }

                __m128 I = _mm_mul_ps (_mm_sqrt_ps (_mm_sqrt_ps (_mm_div_ps (_mm_cvtepi32_ps (N), nmax))), _255);
                
                for (int i = 0; i < 4; i++)
                    {
                    int*   pn = (int*)   &N;
                    float* pI = (float*) &I;

                    BYTE    c     = (BYTE) pI[i];
                    RGBQUAD color = (pn[i] < nMax)? RGBQUAD { (BYTE) (255-c), (BYTE) (c%2 * 64), c } : RGBQUAD {};

                    scr[iy][ix+i] = color;
                    }
                }
            }
            
        printf ("\t\r%.0lf", txGetFPS());
        txSleep();
        }
    }
