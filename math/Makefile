all: permsqrt parity.o

permsqrt: permutation.o
	gcc permutation.o permsqrt.c -o permsqrt

parity.o: permutation.o
	gcc -c parity.c -I../ -o _parity.o
	ld -r _parity.o permutation.o -o parity.o
	rm _parity.o

permutation.o:
	gcc -c permutation.c

clean: 
	rm *.o
	rm permsqrt
