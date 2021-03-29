
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
                  
            for (int ix = 0; ix < 800; ix += 4, x0 += dx*4)
                {
                float X0[4] = { x0, x0+dx, x0+dx*2, x0+dx*3 };
                float Y0[4] = { y0, y0,    y0,      y0      };

                int N[4] = {0, 0, 0, 0};

                for (int i = 0; i < 4; i++)
                    {
                    float X = X0[i],
                          Y = Y0[i];

                    for (; N[i] < nMax; N[i]++)
                        {
                        float x2 = X*X,
                              y2 = Y*Y,
                              xy = X*Y;

                        float r2 = x2 + y2;

                        if (r2 >= r2Max) break;

                        X = x2 - y2 + X0[i],
                        Y = xy + xy + Y0[i];
                        }
                    }

                for (int i = 0; i < 4; i++)
                    {
                    float I = sqrtf (sqrtf ((float)N[i] / (float)nMax)) * 255.f;
                    
                    BYTE c = (BYTE) I;
                    RGBQUAD color = (N[i] < nMax)? RGBQUAD { (BYTE) (255-c), (BYTE) (c%2 * 64), c } : RGBQUAD {};
                    scr[iy][ix+i] = color;
                    }
                }
            }
            
        printf ("\t\r%.0lf", txGetFPS());
        txSleep();
        }
    }

