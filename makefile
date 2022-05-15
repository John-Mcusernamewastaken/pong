objects = vector2.o pong.o
include_paths = -IC:/MinGWlib/include/SDL2
library_paths = -LC:/MinGWlib/lib
linker_flags = -lSDL2main -lSDL2
compiler_flags = -mwindows #prevents console window
executable_name = "release/pong"

all : $(objects) #create edit(?) using value of($objects) 
	gcc $(objects) $(include_paths) $(compiler_flags) $(library_paths) $(linker_flags) -o $(executable_name)
	make clean

pong.o :  pong.c #create pong.o using pong.c
	gcc -c pong.c $(include_paths) $(library_paths) $(linker_flags)
	
vector2.o :  vector2.c
	gcc -c vector2.c $(include_paths) $(library_paths) $(linker_flags)
	
clean:
	rm $(objects)