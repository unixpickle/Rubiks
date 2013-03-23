solvesearch: heuristic.o
	gcc -O2 solvesearch.c heuristic.o cubeindex.o ../cube.o ../rubiksmap.o -o solvesearch -I.. -lpthread

heuristic.o: cubeindex.o heuristic.c
	gcc -O2 -c heuristic.c -I..

cubeindex.o: cubeindex.c
	gcc -O2 -c cubeindex.c -I..

clean:
	rm *.o
	rm solvesearch
