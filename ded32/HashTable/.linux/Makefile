CXX = g++
LD = g++
NASM = nasm

C_FLAGS = -O1 -Wall -std=gnu++20 -m64 -masm=intel -fexceptions -march=native -mfpmath=sse -g
L_FLAGS = -m64
A_FLAGS = -f elf64

RUN = ./ht -v -t ../dict-en-rus.dic


.PHONY : all clear re run profile gdb


all : filebuf.o general.o hashtable.o main.o fnv1a.obj
	$(LD) $(L_FLAGS) -o ht filebuf.o general.o hashtable.o main.o fnv1a.obj

%.o : ../%.cpp
	$(CXX) $(C_FLAGS) -c $< -o $@

../filebuf.cpp : ../filebuf.h
../filebuf.h : ../general.h
../general.cpp : ../general.h
../hashtable.cpp : ../hashtable.h
../hashtable.h : ../general.h ../filebuf.h ../checksum.h
../main.cpp : ../general.h ../filebuf.h ../hashtable.h

fnv1a.obj : ../fnv1a_linux.asm
	$(NASM) $(A_FLAGS) ../fnv1a_linux.asm -o fnv1a.obj

clean :
	rm *.o

re : clean all

run : all
	$(RUN)

gdb : all
	gdb --args $(RUN)

profile : all
	valgrind --tool=callgrind --dump-instr=yes $(RUN)