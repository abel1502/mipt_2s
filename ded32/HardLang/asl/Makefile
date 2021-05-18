NAME		= asl

MSVC_DIR 	= C:/Program Files/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.26.28801
WINLIB_DIR	= C:/Windows Kits/10/Lib/10.0.18362.0/

NASM		= ../../Asm32/Asm32/nasm
LIB			= "$(MSVC_DIR)/bin/Hostx64/x64/lib"

# TODO: -O ?
ASM_FLAGS	= -f win64
			
USED_LIBS	= \
#			  libucrt.lib \
#			  libcmt.lib

LIB_FLAGS	= -machine:x64 						\
			  -libpath:"$(MSVC_DIR)/lib/x64" 	\
			  -libpath:"$(WINLIB_DIR)ucrt/x64/" \
			  -libpath:"$(WINLIB_DIR)um/x64/"


# ------------------------------------------

.PHONY : all clean

ASM_NAME 	= $(NAME).s
OBJ_NAME 	= $(NAME).o
LIB_NAME 	= $(NAME).lib


all :	$(LIB_NAME)

$(LIB_NAME) :	$(OBJ_NAME)
	$(LIB) $(LIB_FLAGS) $(USED_LIBS) -out:$(LIB_NAME) $(OBJ_NAME)

$(OBJ_NAME) :	$(ASM_NAME)
	$(NASM) $(ASM_NAME) $(ASM_FLAGS) -o $(OBJ_NAME) -l $(OBJ_NAME).lst

clean :
	wsl -- rm -f *.o
	wsl -- rm -f *.lst



