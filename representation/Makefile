main: cube.o rubiksmap.o stickermap.o

cube.o: rubiksmap.o
	gcc -c cube.c

rubiksmap.o: stickermap.o
	gcc -c rubiksmap.c

stickermap.o:
	gcc -c stickermap.c

clean:
	rm *.o
