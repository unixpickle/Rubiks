main: rubiksmap.o cube.o brutesolve facesolve

brutesolve: rubiksmap.o cube.o cubesearch.o
	gcc brutesolve.c rubiksmap.o cube.o cubesearch.o -o brutesolve -lpthread

facesolve: rubiksmap.o cube.o cubesearch.o
	gcc facesolve.c rubiksmap.o cube.o cubesearch.o -o facesolve -lpthread

cubesearch.o: cubesearch.c cube.o
	gcc -c cubesearch.c -lpthread

rubiksmap.o: rubiksmap.c
	gcc -c rubiksmap.c

cube.o: cube.c
	gcc -c cube.c

clean:
	rm *.o
	rm brutesolve
	rm facesolve
