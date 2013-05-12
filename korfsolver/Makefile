solvesearch: heuristic.o
	gcc -O2 solvesearch.c heuristic.o ../representation/cube.o ../indexing/index.o -o solvesearch -I../ -lpthread

heuristic.o: heuristic.c
	gcc -O2 -c heuristic.c -I../

clean:
	rm *.o
	rm solvesearch
