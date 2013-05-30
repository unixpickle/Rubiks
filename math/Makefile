all: permsqrt

permsqrt: permutation.o
	gcc permutation.o permsqrt.c -o permsqrt

permutation.o:
	gcc -c permutation.c

clean: 
	rm *.o
	rm permsqrt
