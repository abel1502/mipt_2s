# HashTable

A highly optimized hashtable implementation in limited C++ and x86-64 assembler,

## Key features
 - Optimized by 60% - detailed optimization log provided in `./optimization-log/Log.pdf`
 - Open addressing with linear probing for cache friendliness
 - Supports string keys not longer than 64 characters for performance reasons
 - Compatible with both Linux and Windows
