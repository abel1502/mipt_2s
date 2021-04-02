#ifndef FILEMAP_H
#define FILEMAP_H


/**
 * A read-only file mapping. (A wrapper around the corresponding windows api)
 */
class FileMap {
public:
    FileMap();

    FileMap(const char *name);

    FileMap(const FileMap &other) = delete;

    FileMap &operator=(const FileMap &other) = delete;

    ~FileMap() noexcept;

    /**
     * Returns isLoaded() after the operation (except if the file was preloaded - then false)
     */
    bool load(const char *name);

    /**
     * Returns isLoaded() after the operation (including if the file was not loaded)
     */
    bool close();

    bool isLoaded() const;

private:
    bool loaded;

    HANDLE hFile;
    HANDLE hMapping;
    LPVOID lpMapping;

};

#endif
