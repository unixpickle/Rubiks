main: brutesolve facesolve

brutesolve: cubesearch.o
	gcc -I../representation -O2 brutesolve.c cubesearch.o ../representation/cube.o -o brutesolve -lpthread

facesolve: cubesearch.o
	gcc -I../representation -O2 facesolve.c cubesearch.o ../representation/cube.o -o facesolve -lpthread

cubesearch.o: cubesearch.c
	gcc -O2 -c cubesearch.c -I../representation -lpthread

clean:
	rm *.o
	rm brutesolve
	rm facesolve
