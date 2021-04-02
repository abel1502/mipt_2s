#include <TXLib.h>

#include "filemap.h"


FileMap::FileMap() : 
    loaded{false},
    hFile{NULL},
    hMapping{NULL},
    lpMapping{nullptr} {}

FileMap::FileMap(const char *name) :
    FileMap() {

    load(name);
}

FileMap::~FileMap() noexcept {
    if (loaded) {
        UnmapViewOfFile(lpMapping);
        CloseHandle(hMapping);
        CloseHandle(hFile);
    }
}

bool FileMap::load(const char *name) {
    if (loaded) {
        return false;
    }

    hFile = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (!hFile) {
        return false;
    }

    hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);

    if (!hMapping) {
        return false;
    }

    lpMapping = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);

    if (!lpMapping) {
        return false;
    }

    return loaded = true;
}

bool FileMap::close() {
    if (!loaded)
        return false;

    bool result =
        UnmapViewOfFile(lpMapping) &&
        CloseHandle(hMapping) &&
        CloseHandle(hFile);

    hFile = NULL;
    hMapping = NULL;
    lpMapping = nullptr;

    return loaded = !result;
}

bool FileMap::isLoaded() const {
    return loaded;
}