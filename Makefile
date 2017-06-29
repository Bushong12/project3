
all: mandel mandelmovie

mandel: mandel.o bitmap.o 
	gcc mandel.o bitmap.o -o mandel -lpthread

mandelmovie: mandelmovie.o
	/usr/bin/gcc mandelmovie.o -o mandelmovie

mandel.o: mandel.c
	/usr/bin/gcc -std=c99 -Wall -g -c mandel.c -o mandel.o

bitmap.o: bitmap.c
	/usr/bin/gcc -Wall -g -c bitmap.c -o bitmap.o

mandelmovie.o: mandelmovie.c
	/usr/bin/gcc -std=c99 -Wall -g -c mandelmovie.c -o mandelmovie.o

clean:
	rm -f mandel.o bitmap.o mandelmovie.o mandel mandelmovie
