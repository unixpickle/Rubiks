solver: rubikspl_cmd.o
	gcc -O2 -lpthread solver.c rubikspl_cmd.o heuristic.o solveagent.o ../representation/cube.o ../indexing/index.o -I../ -o solver

rubikspl_cmd.o: rubikspl.o solveagent.o
	gcc -O2 -c rubikspl_cmd.c -o _rubikspl_cmd.o -I../
	ld -r _rubikspl_cmd.o rubikspl.o -o rubikspl_cmd.o
	rm _rubikspl_cmd.o

rubikspl.o: heuristic.o
	gcc -O2 -c rubikspl.c -I../

solveagent.o: solveagent.c
	gcc -O2 -c solveagent.c

heuristic.o: heuristic.c
	gcc -O2 -c heuristic.c -I../

clean:
	rm *.o
	rm solver
