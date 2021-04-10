#include <cassert>
#include <chrono>

#include "picture.h"


using namespace sse;


PictureDisplay::PictureDisplay() :
    PictureDisplay(L"./table.bmp", L"./cat.bmp") {}

PictureDisplay::PictureDisplay(const wchar_t *tableFileName, const wchar_t *catFileName) :
    gdiToken{NULL},
    tableBitmap{nullptr},
    table{},
    catBitmap{nullptr},
    cat{},
    catX{CAT_X_DEFAULT},
    catY{CAT_Y_DEFAULT} {

    Gdiplus::GdiplusStartupInput gdiplusStartupInput{};
    Gdiplus::GdiplusStartup(&gdiToken, &gdiplusStartupInput, NULL);

    tableBitmap = Gdiplus::Bitmap::FromFile(tableFileName);
    catBitmap = Gdiplus::Bitmap::FromFile(catFileName);

    assert(tableBitmap->GetLastStatus() == Gdiplus::Ok);
    assert(catBitmap->GetLastStatus() == Gdiplus::Ok);

    Gdiplus::Rect bounds{0, 0, (INT)tableBitmap->GetWidth(), (INT)tableBitmap->GetHeight()};

    assert(bounds.Height == WND_HEIGHT && bounds.Width == WND_WIDTH);

    if (tableBitmap->LockBits(&bounds, Gdiplus::ImageLockModeRead /*| Gdiplus::ImageLockModeWrite*/, PixelFormat32bppARGB, &table) != Gdiplus::Ok) {
        abort();
    }

    bounds = {0, 0, (INT)catBitmap->GetWidth(), (INT)catBitmap->GetHeight()};

    assert(bounds.Height == CAT_HEIGHT && bounds.Width == CAT_WIDTH);

    if (catBitmap->LockBits(&bounds, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &cat) != Gdiplus::Ok) {
        abort();
    }
}

PictureDisplay::~PictureDisplay() noexcept {
    if (tableBitmap)
        tableBitmap->UnlockBits(&table);
    if (catBitmap)
        catBitmap->UnlockBits(&cat);

    Gdiplus::GdiplusShutdown(gdiToken);
}

bool PictureDisplay::render(screen_t screen, const bool perfCnt) const {
    for (unsigned iy = 0; iy < WND_HEIGHT; ++iy) {
        if (GetAsyncKeyState(VK_ESCAPE)) return true;

        for (unsigned ix = 0; ix < WND_WIDTH; ix += 8) {
            c32 pixels = _mm256_load_si256((__m256i *)((RGBQUAD *)table.Scan0 + (size_t)iy * WND_WIDTH + ix));

            if (!perfCnt)
                _mm256_store_si256((__m256i *)&screen[WND_HEIGHT - 1 - iy][ix], pixels);
        }
    }

    for (unsigned iy = 0; iy < CAT_HEIGHT; ++iy) {
        if (GetAsyncKeyState(VK_ESCAPE)) return true;

        for (unsigned ix = 0; ix < CAT_WIDTH; ix += 8) {
            c32 pixels = _mm256_load_si256((__m256i *)((RGBQUAD *)table.Scan0 + ((size_t)iy + catY) * WND_WIDTH + ix + catX));
            c32 catPixels = _mm256_load_si256((__m256i *)((RGBQUAD *)cat.Scan0 + (size_t)iy * CAT_WIDTH + ix));

            // TODO: Measure performance here?

            c32 coeff = _mm256_shuffle_epi8(catPixels, c32{31, 31, 31, 31, 27, 27, 27, 27, 23, 23, 23, 23, 19, 19, 19, 19, 15, 15, 15, 15, 11, 11, 11, 11, 7, 7, 7, 7, 3, 3, 3, 3});

            __m256i pixelsLow = _mm256_cvtepu8_epi16(catPixels.cast_c16());
            __m256i pixelsHigh = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(catPixels, 1));

            pixelsLow = _mm256_srli_epi16(_mm256_mullo_epi16(pixelsLow, _mm256_cvtepu8_epi16(_mm256_extracti128_si256(coeff, 0))), 8);
            pixelsHigh = _mm256_srli_epi16(_mm256_mullo_epi16(pixelsHigh, _mm256_cvtepu8_epi16(_mm256_extracti128_si256(coeff, 1))), 8);

            catPixels = _mm256_permute4x64_epi64(_mm256_packus_epi16(pixelsLow, pixelsHigh), 0xd8);

            pixelsLow = _mm256_cvtepu8_epi16(pixels.cast_c16());
            pixelsHigh = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(pixels, 1));

            coeff = /*_mm256_setzero_si256()*/ c32{-1} - coeff;

            pixelsLow = _mm256_srli_epi16(_mm256_mullo_epi16(pixelsLow, _mm256_cvtepu8_epi16(_mm256_extracti128_si256(coeff, 0))), 8);
            pixelsHigh = _mm256_srli_epi16(_mm256_mullo_epi16(pixelsHigh, _mm256_cvtepu8_epi16(_mm256_extracti128_si256(coeff, 1))), 8);

            pixels = _mm256_permute4x64_epi64(_mm256_packus_epi16(pixelsLow, pixelsHigh), 0xd8);

            if (!perfCnt)
                _mm256_store_si256((__m256i *)&screen[WND_HEIGHT - 1 - (catY + iy)][catX + ix], catPixels + pixels);
            else
                volatile int tmp = _mm256_movemask_epi8(catPixels + pixels);
        }
    }

    return false;
}

void PictureDisplay::renderLoop() {
    txCreateWindow(WND_WIDTH, WND_HEIGHT);
    Win32::_fpreset();
    txBegin();

    screen_t screen = (screen_t)*txVideoMemory();

    while (true) {
        int shiftX = 0, shiftY = 0;

        constexpr unsigned VSPEED = 4;
        constexpr unsigned HSPEED = 8;

        if (GetAsyncKeyState('A') & 0x8001) shiftX -= HSPEED;
        if (GetAsyncKeyState('D') & 0x8001) shiftX += HSPEED;
        if (GetAsyncKeyState('W') & 0x8001) shiftY -= VSPEED;
        if (GetAsyncKeyState('S') & 0x8001) shiftY += VSPEED;

        shift(shiftX, shiftY);

        if (render(screen))
            break;

        printf("\t\r%.3lf", txGetFPS());

        txSleep();
    }

    txEnd();
}

void PictureDisplay::perfCount() const {
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

void PictureDisplay::shift(int distX, int distY) {
    if ((int)catX + distX >= 0 && (int)catX + CAT_WIDTH + distX <= WND_WIDTH) {
        catX += distX;
    }

    if ((int)catY + distY >= 0 && (int)catY + CAT_HEIGHT + distY <= WND_HEIGHT) {
        catY += distY;
    }
}
