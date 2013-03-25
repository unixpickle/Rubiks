main: indexmaker cornergen tableformat tableformat2

indexmaker: configcounter.o
	gcc configcounter.o ../representation/stickermap.o indexmaker.c -o indexmaker -I../representation

cornergen: cornergen.c
	gcc cornergen.c ../representation/stickermap.o -o cornergen -I../representation

tableformat: tableformat.c
	gcc -D_FILE_OFFSET_BITS=64 tableformat.c -o tableformat

tableformat2: tableformat2.c
	gcc -D_FILE_OFFSET_BITS=64 tableformat2.c ../representation/symmetry.o -o tableformat2 -I../representation

configcounter.o: configcounter.c
	gcc -c configcounter.c -I../representation

clean:
	rm *.o
	rm indexmaker
	rm cornergen
	rm tableformat
