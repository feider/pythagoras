CC=gcc
INCLUDES=`sdl-config --cflags`
LIBS=./3rdparty/vecmath/vecmath/vecmath.c -lm `sdl-config --libs`

pythagoras: src/main.c
	$(CC) src/main.c $(INCLUDES) $(LIBS) -o pythagoras
