
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

                float X [4] = {}; for (int i = 0; i < 4; i++) X[i] = X0[i];
                float Y [4] = {}; for (int i = 0; i < 4; i++) Y[i] = Y0[i];
                      
                int N[4] = {0, 0, 0, 0};

                for (int n = 0; n < nMax; n++)
                    {
                    float x2[4] = {}; for (int i = 0; i < 4; i++) x2[i] =  X[i] *  X[i];
                    float y2[4] = {}; for (int i = 0; i < 4; i++) y2[i] =  Y[i] *  Y[i];
                    float xy[4] = {}; for (int i = 0; i < 4; i++) xy[i] =  X[i] *  Y[i];

                    float r2[4] = {}; for (int i = 0; i < 4; i++) r2[i] = x2[i] + y2[i];
                    
                    int cmp[4] = {};
                    for (int i = 0; i < 4; i++) if (r2[i] <= r2Max) cmp[i] = 1;

                    int mask = 0;
                    for (int i = 0; i < 4; i++) mask |= (cmp[i] << i);
                    if (!mask) break;
                    
                    for (int i = 0; i < 4; i++) N[i] = N[i] + cmp[i];

                    for (int i = 0; i < 4; i++) X[i] = x2[i] - y2[i] + X0[i];
                    for (int i = 0; i < 4; i++) Y[i] = xy[i] + xy[i] + Y0[i];
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
