# Compiler
CC = g++

# Source files
SRCS = main.cpp LTexture.hpp Dot.hpp Ball.hpp

# Object files (không cần .o vì ta không tách header và implementation)
OBJS = $(SRCS)

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -ID:\HCMUT\HK6\Game_Programing\BTL\Assignment_2\x86_64-w64-mingw32_sdl\include\SDL2 \
				-ID:\HCMUT\HK6\Game_Programing\BTL\Assignment_2\x86_64-w64-mingw32_sld_image\include\SDL2 \
				-ID:\HCMUT\HK6\Game_Programing\BTL\Assignment_2\x86_64-w64-mingw32_sdll_ttf\include\SDL2 \
				-ID:\HCMUT\HK6\Game_Programing\BTL\Assignment_2\x86_64-w64-mingw32_sdl_mixer\include\SDL2
INCLUDE_PATHS = -ID:\Monhoc\LTgame\BTL\BTL2\BTL_Game_2\x86_64-w64-mingw32_sdl\include\SDL2 \
				-ID:\Monhoc\LTgame\BTL\BTL2\BTL_Game_2\x86_64-w64-mingw32_sdl_mixer\include\SDL2 \
				-ID:\Monhoc\LTgame\BTL\BTL2\BTL_Game_2\x86_64-w64-mingw32_sdll_ttf\include\SDL2 \
				-ID:\Monhoc\LTgame\BTL\BTL2\BTL_Game_2\x86_64-w64-mingw32_sld_image\include\SDL2

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -LD:\Monhoc\LTgame\BTL\BTL2\BTL_Game_2\x86_64-w64-mingw32_sdl\lib \
				-LD:\Monhoc\LTgame\BTL\BTL2\BTL_Game_2\x86_64-w64-mingw32_sdl_mixer\lib \
				-LD:\Monhoc\LTgame\BTL\BTL2\BTL_Game_2\x86_64-w64-mingw32_sdll_ttf\lib \
				-LD:\Monhoc\LTgame\BTL\BTL2\BTL_Game_2\x86_64-w64-mingw32_sld_image\lib

# Compiler flags
COMPILER_FLAGS = -w -Wl,-subsystem,windows

# Linker flags
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# Executable file
OBJ_NAME = game.exe

# Build target
all: $(SRCS)
	$(CC) $(SRCS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

# Clean target
clean:
	rm -f $(OBJ_NAME)