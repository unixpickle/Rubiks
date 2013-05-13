pocketsolve: pocketmap.o pocketsolve.c
	gcc pocketsolve.c pocketmap.o ../indexing/index.o ../representation/cube.o -I../ -o pocketsolve

pocketmap.o: pocketmap.c
	gcc -c pocketmap.c -I../

clean:
	rm *.o
	rm pocketsolve
