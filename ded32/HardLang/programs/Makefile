PROGRAMS = fib solvesquare test
SRC_PATH = ./
OBJ_PATH = ./
EXE_PATH = ./

ALC = ../bin/Release/HardLang.exe
# TODO: ?
LD = g++

ALC_FLAGS = -vvv
LD_FLAGS = 

.SECONDEXPANSION :

.PHONY : $(PROGRAMS)

$(addprefix $(OBJ_PATH),$(addsuffix .o,$(PROGRAMS))) : $(SRC_PATH)$$(basename $$@).asf
	$(ALC) $(ALC_FLAGS) -i $@ -o $^

$(addprefix $(EXE_PATH),$(addsuffix .exe,$(PROGRAMS))) : $(OBJ_PATH)$$(basename $$@).o
	$(LD) $(LD_FLAGS) $^

$(PROGRAMS) : $(EXE_PATH)$$@.exe
	$^
