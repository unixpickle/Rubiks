solver: rubikspl_cmd.o orientpl_cmd.o
	gcc -O2 -lpthread solver.c rubikspl_cmd.o orientpl_cmd.o cmdutil.o heuristic.o solveagent.o ../representation/cube.o ../indexing/index.o -I../ -o solver

orientpl_cmd.o: orientpl.o solveagent.o cmdutil.o
	gcc -O2 -c orientpl_cmd.c -o _orientpl_cmd.o -I../
	ld -r _orientpl_cmd.o orientpl.o -o orientpl_cmd.o
	rm _orientpl_cmd.o

rubikspl_cmd.o: rubikspl.o solveagent.o cmdutil.o
	gcc -O2 -c rubikspl_cmd.c -o _rubikspl_cmd.o -I../
	ld -r _rubikspl_cmd.o rubikspl.o -o rubikspl_cmd.o
	rm _rubikspl_cmd.o

cmdutil.o:
	gcc -O2 -c cmdutil.c

orientpl.o: heuristic.o
	gcc -O2 -c orientpl.c -I../

rubikspl.o: heuristic.o
	gcc -O2 -c rubikspl.c -I../

solveagent.o: solveagent.c
	gcc -O2 -c solveagent.c

heuristic.o: heuristic.c
	gcc -O2 -c heuristic.c -I../

clean:
	rm *.o
	rm solver
