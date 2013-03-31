pocketsolve: pocketmap.o pocketsolve.c
	gcc pocketsolve.c pocketmap.o ../indexing/index.o ../representation/cube.o -I../indexing -I../representation -o pocketsolve

pocketmap.o: pocketmap.c
	gcc -c pocketmap.c -I../representation

clean:
	rm *.o
	rm pocketsolve
