solvesearch: heuristic.o
	gcc solvesearch.c heuristic.o cubeindex.o ../cube.o ../rubiksmap.o -o solvesearch -I.. -lpthread

heuristic.o: cubeindex.o heuristic.c
	gcc -c heuristic.c -I..

cubeindex.o: cubeindex.c
	gcc -c cubeindex.c -I..

clean:
	rm *.o
	rm solvesearch
