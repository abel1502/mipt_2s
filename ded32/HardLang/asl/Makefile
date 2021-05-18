NAME		= asl

MSVC_DIR 	= C:/Program Files/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.26.28801
WINLIB_DIR	= C:/Windows Kits/10/Lib/10.0.18362.0/

NASM		= ../../Asm32/Asm32/nasm
LIB			= "$(MSVC_DIR)/bin/Hostx64/x64/lib"

# TODO: -O ?
ASM_FLAGS	= -f win64
			
USED_LIBS	= legacy_stdio_definitions.lib

LOADER_LIBS = libucrt.lib libcmt.lib

LIB_FLAGS	= -machine:x64 						\
			  -libpath:"$(MSVC_DIR)/lib/x64" 	\
			  -libpath:"$(WINLIB_DIR)ucrt/x64/" \
			  -libpath:"$(WINLIB_DIR)um/x64/"


# ------------------------------------------

.PHONY : all clean

ASM_NAME 		= $(NAME).s
ASM_LOADER_NAME = $(NAME)_loader.s
OBJ_NAME 		= $(NAME).o
OBJ_LOADER_NAME = $(NAME)_loader.o
LIB_NAME 		= $(NAME).lib
LIB_LOADER_NAME = $(NAME)_loader.lib


all :	$(LIB_NAME) $(LIB_LOADER_NAME)

$(LIB_NAME) :	$(OBJ_NAME)
	$(LIB) $(LIB_FLAGS) $(USED_LIBS) -out:$(LIB_NAME) $(OBJ_NAME)

$(LIB_LOADER_NAME) :	$(OBJ_LOADER_NAME)
	$(LIB) $(LIB_FLAGS) $(LOADER_LIBS) -out:$(LIB_LOADER_NAME) $(OBJ_LOADER_NAME)

$(OBJ_NAME) :	$(ASM_NAME)
	$(NASM) $(ASM_NAME) $(ASM_FLAGS) -o $(OBJ_NAME)

$(OBJ_LOADER_NAME) :	$(ASM_LOADER_NAME)
	$(NASM) $(ASM_LOADER_NAME) $(ASM_FLAGS) -o $(OBJ_LOADER_NAME)

clean :
	wsl -- rm -f *.o
	wsl -- rm -f *.lst



