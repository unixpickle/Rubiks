main: rubiksmap.o cube.o brutesolve

brutesolve: rubiksmap.o cube.o
	gcc brutesolve.c rubiksmap.o cube.o -o brutesolve

rubiksmap.o: rubiksmap.c
	gcc -c rubiksmap.c

cube.o: cube.c
	gcc -c cube.c

clean:
	rm *.o
	rm brutesolve
