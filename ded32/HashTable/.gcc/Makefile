CXX = g++
LD = g++
NASM = ../../Asm32/Asm32/nasm

C_FLAGS = -O0 -Wall -std=gnu++20 -m64 -masm=intel -fexceptions -march=native -mfpmath=sse -g
L_FLAGS = -m64
A_FLAGS = -f win64


.PHONY : all clear


all : filebuf.o general.o hashtable.o main.o fnv1a.obj
	$(LD) $(L_FLAGS) -o ht.exe filebuf.o general.o hashtable.o main.o fnv1a.obj

%.o : ../%.cpp
	$(CXX) $(C_FLAGS) -c $< -o $@

fnv1a.obj : ../fnv1a.asm
	$(NASM) $(A_FLAGS) ../fnv1a.asm -o fnv1a.obj

clean :
	wsl -- rm *.o

run : all
	./ht -v -t ../dict-en-rus.dic

