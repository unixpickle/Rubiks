rootfinder: maproot.o
	gcc rootfinder.c maproot.o ../../representation/cube.o ../../util/cmdutil.o ../../util/pltools.o ../cubeperm.o -I../../ -o rootfinder

maproot.o:
	gcc -c maproot.c -I../../

clean:
	rm *.o
	rm rootfinder
