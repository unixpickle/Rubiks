solver: rubikspl_cmd.o orientpl_cmd.o blockpl_cmd.o heuristic.o pltools.o
	gcc -O2 -lpthread solver.c rubikspl_cmd.o orientpl_cmd.o blockpl_cmd.o pltools.o cmdutil.o heuristic.o solveagent.o ../representation/cube.o ../indexing/index.o -I../ -o solver
	rm *.o

pltools.o:
	gcc -O2 -c pltools.c

blockpl_cmd.o: solveagent.o cmdutil.o
	cd blockpl && make
	mv blockpl/blockpl_cmd.o .
	rm blockpl/*.o

orientpl_cmd.o: solveagent.o cmdutil.o
	cd orientpl && make
	mv orientpl/orientpl_cmd.o .
	rm orientpl/*.o

rubikspl_cmd.o: solveagent.o cmdutil.o
	cd rubikspl && make
	mv rubikspl/rubikspl_cmd.o .
	rm rubikspl/*.o

cmdutil.o:
	gcc -O2 -c cmdutil.c -I../

solveagent.o: solveagent.c
	gcc -O2 -c solveagent.c

heuristic.o: heuristic.c
	gcc -O2 -c heuristic.c -I../

clean:
	rm *.o
	rm solver
