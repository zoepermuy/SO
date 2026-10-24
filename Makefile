CC = gcc
CFLAGS = -g -Wall

p2: p2.c start.o path.o listasimple.o fichabiertos.o memoria.o
	$(CC) -o p2 $(CFLAGS) p2.c start.o path.o listasimple.o fichabiertos.o memoria.o

start.o: start.c start.h path.h memoria.h
	$(CC) -c $(CFLAGS) start.c

fichabiertos.o: fichabiertos.c fichabiertos.h
	$(CC) -c $(CFLAGS) fichabiertos.c

path.o: path.c path.h listasimple.h
	$(CC) -c $(CFLAGS) path.c

listasimple.o: listasimple.c listasimple.h
	$(CC) -c $(CFLAGS) listasimple.c

memoria.o: memoria.c memoria.h
	$(CC) -c $(CFLAGS) memoria.c

clean:
	rm p2 *.o
