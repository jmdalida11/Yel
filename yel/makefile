INCLUDE_PATHS = -IC:\MinGW\include
LIBRARY_PATHS = -LC:\MinGW\lib

COMPILER_FLAGS = -w -Wl,-subsystem,windows
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lglu32

OBJ_NAME = Yel
OBJS = main.cpp board.cpp eval.cpp search.cpp gui.cpp tile.cpp piece.cpp

make:
	g++ $(OBJS) -o $(OBJ_NAME) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS)
#mingw32-make make