main: indexmaker cornergen tableformat

indexmaker: configcounter.o
	gcc configcounter.o ../cube.o ../rubiksmap.o indexmaker.c -o indexmaker -I..

cornergen: cornergen.c
	gcc cornergen.c ../cube.o ../rubiksmap.o -o cornergen -I..

tableformat: tableformat.c
	gcc tableformat.c -o tableformat

configcounter.o: configcounter.c
	gcc -c configcounter.c -I..

clean:
	rm *.o
	rm indexmaker
	rm cornergen
