
#include "TXLib.h"

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

    const int   nMax  = 256;
    const float dx    = 1/800.f, dy = 1/800.f;
    const float r2Max = 100.f;

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

            for (int ix = 0; ix < 800; ix++, x0 += dx)
                {
                float X = x0, 
                      Y = y0;

                int N = 0;
                for (; N < nMax; N++)
                    {
                    float x2 = X*X,
                          y2 = Y*Y,
                          xy = X*Y;

                    float r2 = x2 + y2;
                       
                    if (r2 >= r2Max) break;
                        
                    X = x2 - y2 + x0,
                    Y = xy + xy + y0;
                    }
                
                float I = sqrtf (sqrtf ((float)N / (float)nMax)) * 255.f;

                BYTE c = (BYTE) I;
                RGBQUAD color = (N < nMax)? RGBQUAD { (BYTE) (255-c), (BYTE) (c%2 * 64), c } : RGBQUAD {};
                scr[iy][ix] = color;
                }
            }
            
        printf ("\t\r%.0lf", txGetFPS());
        txSleep();
        }
    }

