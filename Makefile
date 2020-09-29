CC=gcc
LIBS=-lm -lSDL2 -lSDL2_ttf

pythagoras: src/pythagoras.c
	$(CC) src/pythagoras.c $(LIBS) -o pythagoras
