#include <cassert>

#include "picture.h"


int main(int argc, char** argv) {
    /*ULONG_PTR token = NULL;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput{};

    Gdiplus::GdiplusStartup(&token, &gdiplusStartupInput, NULL);*/

    if (argc < 3) {
        printf("Usage: %s <table> <cat> [-t]\n", argv[0]);

        return 1;
    }

    wchar_t *tableName = (wchar_t *)calloc(strlen(argv[1]) + 1, sizeof(wchar_t));
    wchar_t *catName = (wchar_t *)calloc(strlen(argv[2]) + 1, sizeof(wchar_t));

    for (unsigned i = 0; argv[1][i]; ++i) {
        tableName[i] = argv[1][i];
    }

    for (unsigned i = 0; argv[2][i]; ++i) {
        catName[i] = argv[2][i];
    }

    PictureDisplay pd{tableName, catName};

    if (argc >= 4 && strcmp(argv[3], "-t") == 0) {
        pd.perfCount();
    }
    else {
        pd.renderLoop();
    }

    free(tableName);
    free(catName);

    return 0;
}
