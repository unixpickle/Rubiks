main: rubiksmap.o cube.o brutesolve facesolve indexing_build

brutesolve: rubiksmap.o cube.o cubesearch.o
	gcc -O2 brutesolve.c rubiksmap.o cube.o cubesearch.o -o brutesolve -lpthread

facesolve: rubiksmap.o cube.o cubesearch.o
	gcc -O2 facesolve.c rubiksmap.o cube.o cubesearch.o -o facesolve -lpthread

indexing_build: cube.o
	cd indexing && make

cubesearch.o: cubesearch.c cube.o
	gcc -O2 -c cubesearch.c -lpthread

rubiksmap.o: rubiksmap.c
	gcc -O2 -c rubiksmap.c

cube.o: cube.c rubiksmap.o
	gcc -O2 -c cube.c

clean:
	rm *.o
	rm brutesolve
	rm facesolve
	cd indexing && make clean
