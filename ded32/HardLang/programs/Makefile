PROGRAMS = fib solvesquare test
SRC_PATH = ./
OBJ_PATH = ./
EXE_PATH = ./

ASL			= ../asl/asl.lib
ASL_LOADER	= ../asl/asl_loader.lib

MSVC_DIR = C:/Program Files/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.26.28801
WINLIB_DIR	= C:/Windows Kits/10/Lib/10.0.18362.0/
WININCL_DIR	= C:/Windows Kits/10/Include/10.0.18362.0/ucrt/

# TODO: Change to Release
ALC      = ../bin/Debug/HardLang.exe
LINK     = $(MSVC_DIR)/bin/Hostx64/x64/link

ALC_FLAGS = -vvv
LINK_FLAGS = -entry:mainCRTStartup				\
			 -debug								\
			 -machine:x64						\
			 -subsystem:console					\
			 -opt:ref							\
			 -ignore:4099						\
			 -libpath:"$(MSVC_DIR)/lib/x64" 	\
			 -libpath:"$(WINLIB_DIR)ucrt/x64/"	\
			 -libpath:"$(WINLIB_DIR)um/x64/"

.SECONDEXPANSION :

.PHONY : $(PROGRAMS) clean testLink testCSolvesquare

$(addprefix $(OBJ_PATH),$(addsuffix .o,$(PROGRAMS))) : $(SRC_PATH)$$(basename $$@).asf
	$(ALC) $(ALC_FLAGS) -i $^ -o $@

$(addprefix $(EXE_PATH),$(addsuffix .exe,$(PROGRAMS))) : $(OBJ_PATH)$$(basename $$@).o $(ASL)
	$(LINK) $(LINK_FLAGS) $^ $(ASL) $(ASL_LOADER) -out:$@

$(PROGRAMS) : $(EXE_PATH)$$@.exe
	$^

testLink.o : testLink.asf
	$(ALC) $(ALC_FLAGS) -i testLink.asf -o testLink.o

testLink : test.o testLink.o
	$(LINK) $(LINK_FLAGS) test.o testLink.o $(ASL) $(ASL_LOADER) -out:testLink.exe

testCSolvesquare.obj : testCSolvesquare.cpp
	"$(MSVC_DIR)/bin/Hostx64/x64/cl" -I "$(MSVC_DIR)/include" -I "$(WININCL_DIR)" testCSolvesquare.cpp -Fo testCSolvesquare.obj

testCSolvesquare : testCSolvesquare.obj solvesquare.o
	$(LINK) -debug -machine:x64 -subsystem:console -opt:ref \
			-libpath:"$(MSVC_DIR)/lib/x64" -libpath:"$(WINLIB_DIR)ucrt/x64/" \
			-libpath:"$(WINLIB_DIR)um/x64/" legacy_stdio_definitions.lib \
			testCSolvesquare.obj solvesquare.o $(ASL) -out:testCSolvesquare.exe
	
	./testCSolvesquare.exe

clean :
	wsl -- rm -f *.o
	wsl -- rm -f *.exe