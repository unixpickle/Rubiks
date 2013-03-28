solvesearch: heuristic.o
	gcc -O2 solvesearch.c heuristic.o ../representation/cube.o ../indexing/index.o -o solvesearch -I../representation -I../indexing -lpthread

heuristic.o: heuristic.c
	gcc -O2 -c heuristic.c -I../representation -I../indexing

clean:
	rm *.o
	rm solvesearch
