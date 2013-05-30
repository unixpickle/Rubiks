solver: rubikspl_cmd.o orientpl_cmd.o blockpl_cmd.o crosspl_cmd.o r2f2upl_cmd.o heuristic.o cmdhelper.o
	gcc -O2 -lpthread solver.c rubikspl_cmd.o orientpl_cmd.o blockpl_cmd.o crosspl_cmd.o r2f2upl_cmd.o cmdhelper.o ../util/pltools.o ../util/cmdutil.o heuristic.o solveagent.o ../representation/cube.o ../indexing/index.o -I../ -o solver -lpthread
	rm *.o

cmdhelper.o:
	gcc -O2 -c cmdhelper.c -I../

r2f2upl_cmd.o: solveagent.o
	cd r2f2upl && make
	mv r2f2upl/r2f2upl_cmd.o .
	rm r2f2upl/*.o

crosspl_cmd.o: solveagent.o
	cd crosspl && make
	mv crosspl/crosspl_cmd.o .
	rm crosspl/*.o

blockpl_cmd.o: solveagent.o
	cd blockpl && make
	mv blockpl/blockpl_cmd.o .
	rm blockpl/*.o

orientpl_cmd.o: solveagent.o
	cd orientpl && make
	mv orientpl/orientpl_cmd.o .
	rm orientpl/*.o

rubikspl_cmd.o: solveagent.o
	cd rubikspl && make
	mv rubikspl/rubikspl_cmd.o .
	rm rubikspl/*.o

solveagent.o: solveagent.c
	gcc -O2 -c solveagent.c

heuristic.o: heuristic.c
	gcc -O2 -c heuristic.c -I../

clean:
	rm solver
