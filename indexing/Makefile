indexmaker: configcounter.o
	gcc configcounter.o ../cube.o ../rubiksmap.o indexmaker.c -o indexmaker -I..

configcounter.o: configcounter.c
	gcc -c configcounter.c -I..

clean:
	rm *.o
	rm indexmaker
