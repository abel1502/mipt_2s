NAME	= printf
SRC_ASM	= printf.s

# TODO: Fasm
NASM	= ../Asm32/Asm32/nasm
LINK	= "C:\Program Files\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.26.28801\bin\Hostx64\x64\link"

# TODO: -O ?
ASM_FLAGS	= -f win64
LINK_LIBS	= kernel32.lib
LINK_FLAGS	= -entry:_start -subsystem:CONSOLE \
			  -libpath:"C:\Windows Kits\10\Lib\10.0.18362.0\um\x64" \
			  $(LINK_LIBS)


# ------------------------------------------

.PHONY : c cr clean

OBJ_ASM	= $(SRC_ASM:.s=.o)
EXE		= $(NAME).exe


c : $(EXE)


cr : c
	./$(EXE)


$(EXE) :	$(OBJ_ASM)
	$(LINK) $(LINK_FLAGS) -out:$(EXE) $(OBJ_ASM)


%.o :		%.s
	$(NASM) $< $(ASM_FLAGS) -o $@ -l $*.lst


# I was too lazy to figure out a way to make windows rm do this
clean :
	wsl -- rm $(OBJ_ASM)



