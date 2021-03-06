#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = 

INC = 
CFLAGS = -Wall -std=gnu++20 -m64 -fexceptions -msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -mavx -mavx2 -mfpmath=sse -mpopcnt
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = -m64

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB) 
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/Mandelbrot

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O3
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB) 
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/Mandelbrot

INC_DEBUG_AVX2 = $(INC)
CFLAGS_DEBUG_AVX2 = $(CFLAGS) -g -mpreferred-stack-boundary=5 -march=corei7-avx -mtune=corei7-avx -DUSE_AVX2
RESINC_DEBUG_AVX2 = $(RESINC)
RCFLAGS_DEBUG_AVX2 = $(RCFLAGS)
LIBDIR_DEBUG_AVX2 = $(LIBDIR)
LIB_DEBUG_AVX2 = $(LIB) -lSDL2 -lSDL2_ttf
LDFLAGS_DEBUG_AVX2 = $(LDFLAGS)
OBJDIR_DEBUG_AVX2 = obj/DebugAVX2
DEP_DEBUG_AVX2 = 
OUT_DEBUG_AVX2 = bin/DebugAVX2/Mandelbrot

INC_RELEASE_AVX2 = $(INC)
CFLAGS_RELEASE_AVX2 = $(CFLAGS) -O3 -mpreferred-stack-boundary=5 -march=corei7-avx -mtune=corei7-avx -DUSE_AVX2
RESINC_RELEASE_AVX2 = $(RESINC)
RCFLAGS_RELEASE_AVX2 = $(RCFLAGS)
LIBDIR_RELEASE_AVX2 = $(LIBDIR)
LIB_RELEASE_AVX2 = $(LIB) -lSDL2 -lSDL2_ttf
LDFLAGS_RELEASE_AVX2 = $(LDFLAGS) -s
OBJDIR_RELEASE_AVX2 = obj/ReleaseAVX2
DEP_RELEASE_AVX2 = 
OUT_RELEASE_AVX2 = bin/ReleaseAVX2/Mandelbrot

OBJ_DEBUG = $(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/mandelbrot.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/main.o $(OBJDIR_RELEASE)/mandelbrot.o

OBJ_DEBUG_AVX2 = $(OBJDIR_DEBUG_AVX2)/main.o $(OBJDIR_DEBUG_AVX2)/mandelbrot.o

OBJ_RELEASE_AVX2 = $(OBJDIR_RELEASE_AVX2)/main.o $(OBJDIR_RELEASE_AVX2)/mandelbrot.o

all: debug release debug_avx2 release_avx2

clean: clean_debug clean_release clean_debug_avx2 clean_release_avx2

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/main.o: main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.cpp -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/mandelbrot.o: mandelbrot.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c mandelbrot.cpp -o $(OBJDIR_DEBUG)/mandelbrot.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/main.o: main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c main.cpp -o $(OBJDIR_RELEASE)/main.o

$(OBJDIR_RELEASE)/mandelbrot.o: mandelbrot.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c mandelbrot.cpp -o $(OBJDIR_RELEASE)/mandelbrot.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)

before_debug_avx2: 
	test -d bin/DebugAVX2 || mkdir -p bin/DebugAVX2
	test -d $(OBJDIR_DEBUG_AVX2) || mkdir -p $(OBJDIR_DEBUG_AVX2)

after_debug_avx2: 

debug_avx2: before_debug_avx2 out_debug_avx2 after_debug_avx2

out_debug_avx2: before_debug_avx2 $(OBJ_DEBUG_AVX2) $(DEP_DEBUG_AVX2)
	$(LD) $(LIBDIR_DEBUG_AVX2) -o $(OUT_DEBUG_AVX2) $(OBJ_DEBUG_AVX2)  $(LDFLAGS_DEBUG_AVX2) $(LIB_DEBUG_AVX2)

$(OBJDIR_DEBUG_AVX2)/main.o: main.cpp
	$(CXX) $(CFLAGS_DEBUG_AVX2) $(INC_DEBUG_AVX2) -c main.cpp -o $(OBJDIR_DEBUG_AVX2)/main.o

$(OBJDIR_DEBUG_AVX2)/mandelbrot.o: mandelbrot.cpp
	$(CXX) $(CFLAGS_DEBUG_AVX2) $(INC_DEBUG_AVX2) -c mandelbrot.cpp -o $(OBJDIR_DEBUG_AVX2)/mandelbrot.o

clean_debug_avx2: 
	rm -f $(OBJ_DEBUG_AVX2) $(OUT_DEBUG_AVX2)
	rm -rf bin/DebugAVX2
	rm -rf $(OBJDIR_DEBUG_AVX2)

before_release_avx2: 
	test -d bin/ReleaseAVX2 || mkdir -p bin/ReleaseAVX2
	test -d $(OBJDIR_RELEASE_AVX2) || mkdir -p $(OBJDIR_RELEASE_AVX2)

after_release_avx2: 

release_avx2: before_release_avx2 out_release_avx2 after_release_avx2

out_release_avx2: before_release_avx2 $(OBJ_RELEASE_AVX2) $(DEP_RELEASE_AVX2)
	$(LD) $(LIBDIR_RELEASE_AVX2) -o $(OUT_RELEASE_AVX2) $(OBJ_RELEASE_AVX2)  $(LDFLAGS_RELEASE_AVX2) $(LIB_RELEASE_AVX2)

$(OBJDIR_RELEASE_AVX2)/main.o: main.cpp
	$(CXX) $(CFLAGS_RELEASE_AVX2) $(INC_RELEASE_AVX2) -c main.cpp -o $(OBJDIR_RELEASE_AVX2)/main.o

$(OBJDIR_RELEASE_AVX2)/mandelbrot.o: mandelbrot.cpp
	$(CXX) $(CFLAGS_RELEASE_AVX2) $(INC_RELEASE_AVX2) -c mandelbrot.cpp -o $(OBJDIR_RELEASE_AVX2)/mandelbrot.o

clean_release_avx2: 
	rm -f $(OBJ_RELEASE_AVX2) $(OUT_RELEASE_AVX2)
	rm -rf bin/ReleaseAVX2
	rm -rf $(OBJDIR_RELEASE_AVX2)

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release before_debug_avx2 after_debug_avx2 clean_debug_avx2 before_release_avx2 after_release_avx2 clean_release_avx2

