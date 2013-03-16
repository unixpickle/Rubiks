indexmaker: cubecorners.o
	gcc cubecorners.c ../cube.o ../rubiksmap.o indexmaker.c -o indexmaker -I..

cubecorners.o: cubecorners.c
	gcc -c cubecorners.c -I..

clean:
	rm *.o
	rm indexmaker
