# aprintf

A custom printf-like function in x86-64 assembler for Windows

## Key features
 - Supports `%%`, `%c`, `%s`, `%b`, `%o`, `%d`, `%u`, `%x`, `%n`, `%m`.
   (`%b` prints a number in binary, `%m` prints `<meow>` and beeps).
 - Implements an MSVC-style compact jump table
 - Utilizes the WinApi
 - Completely accords with Microsoft's x64 calling convention
