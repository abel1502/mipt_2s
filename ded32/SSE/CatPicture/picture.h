#ifndef PICTURE_H
#define PICTURE_H

#undef UNICODE
#undef _UNICODE
#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
#include <TXLib.h>
#include "sse.h"

class PictureDisplay {
public:
    static constexpr unsigned WND_HEIGHT = 600;
    static constexpr unsigned WND_WIDTH = 800;

    static constexpr unsigned CAT_HEIGHT = 140;
    static constexpr unsigned CAT_WIDTH = 240;

    static constexpr unsigned CAT_Y = (WND_HEIGHT - CAT_HEIGHT) / 2 - 24;
    static constexpr unsigned CAT_X = (WND_WIDTH - CAT_WIDTH) / 2;

    typedef RGBQUAD(&screen_t)[WND_HEIGHT][WND_WIDTH];

    PictureDisplay();
    PictureDisplay(const wchar_t *tableFileName, const wchar_t *catFileName);

    PictureDisplay(const PictureDisplay &other) = delete;
    PictureDisplay &operator=(const PictureDisplay &other) = delete;

    ~PictureDisplay() noexcept;

    bool render(screen_t screen, bool perfCnt = false) const;

    void renderLoop();

    void perfCount() const;

private:
    ULONG_PTR gdiToken;
    Gdiplus::Bitmap *tableBitmap;  // They have to be pointers because I can't init them in my initializer list, since I need to init gdi first
    Gdiplus::BitmapData table;
    Gdiplus::Bitmap *catBitmap;
    Gdiplus::BitmapData cat;

};

#endif
