main: rubiksmap.o cube.o

rubiksmap.o: rubiksmap.c
	gcc -c rubiksmap.c

cube.o: cube.c
	gcc -c cube.c

clean:
	rm *.o
