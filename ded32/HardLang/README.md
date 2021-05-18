# HardLang

This is my final project of the second semester: it's a continuation of the final 
projext from last semester, SoftLang. This time, instead of compiling into my own
assembler, it creates a 64-bit x86 Windows object file. 

## Key features
 - All the language features of the previous project
 - Much higher execution speed (precise tests yet to be done, though)
 - C and C++ compatibility (can be linked with external object files and libraries, 
   as well as can export x64 Windows ABI-compatible functions)
 - Several grammatic additions (such as `export` and `import` keywords for C/C++
   compatiblity)
 - Extra intrinsic functions for features abscent from the language itself, ~~such as
   arrays and raw memory access~~
 - A standard library, including options to be used both with or without Windows UCRT
 - More to come...

## Examples
The `./programs/` folder contains the same examples as in SoftLang, and a couple extra ones

## TODO
This readme remains to be finished...