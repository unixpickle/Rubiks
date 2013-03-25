main: cube.o rubiksmap.o stickermap.o symmetry.o

cube.o: rubiksmap.o
	gcc -c cube.c

rubiksmap.o: stickermap.o
	gcc -c rubiksmap.c

stickermap.o:
	gcc -c stickermap.c

symmetry.o:
	gcc -c symmetry.c

clean:
	rm *.o
