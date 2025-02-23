#OBJS specifies which files to compile as part of the project
OBJS = 26_motion.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -ID:\HCMUT\HK6\Game_Programing\BTL\Assignment_2\x86_64-w64-mingw32_sdl\include\SDL2 \
				-ID:\HCMUT\HK6\Game_Programing\BTL\Assignment_2\x86_64-w64-mingw32_sld_image\include\SDL2 \
				-ID:\HCMUT\HK6\Game_Programing\BTL\Assignment_2\x86_64-w64-mingw32_sdll_ttf\include\SDL2 \
				-ID:\HCMUT\HK6\Game_Programing\BTL\Assignment_2\x86_64-w64-mingw32_sdl_mixer\include\SDL2

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -LD:\HCMUT\HK6\Game_Programing\BTL\Assignment_2\x86_64-w64-mingw32_sdl\lib \
				-LD:\HCMUT\HK6\Game_Programing\BTL\Assignment_2\x86_64-w64-mingw32_sld_image\lib \
				-LD:\HCMUT\HK6\Game_Programing\BTL\Assignment_2\x86_64-w64-mingw32_sdll_ttf\lib \
				-LD:\HCMUT\HK6\Game_Programing\BTL\Assignment_2\x86_64-w64-mingw32_sdl_mixer\lib

#COMPILER_FLAGS specifies the additional compilation options we're using
#-w suppresses all warnings
#-Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -w -Wl,-subsystem,windows

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = 26_tut


#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)