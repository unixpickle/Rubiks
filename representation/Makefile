main: cube.o

cube.o: rubiksmap.o
	gcc -c cube.c -o _cube.o
	ld -r _cube.o rubiksmap.o -o cube.o
	rm _cube.o

rubiksmap.o: stickermap.o symmetry.o
	gcc -c rubiksmap.c -o _rubiksmap.o
	ld -r _rubiksmap.o stickermap.o symmetry.o -o rubiksmap.o
	rm _rubiksmap.o

stickermap.o:
	gcc -c stickermap.c

symmetry.o:
	gcc -c symmetry.c

clean:
	rm *.o
