generator: randcube.o
	gcc generator.c randcube.o ../cubeperm.o ../../representation/cube.o -I../../ -o generator

randcube.o: randperm.o
	gcc -c randcube.c -o _randcube.o -I../../
	ld -r _randcube.o randperm.o -o randcube.o
	rm _randcube.o randperm.o

randperm.o:
	gcc -c randperm.c -I../../

clean:
	rm *.o generator
