main: brutesolve facesolve

brutesolve: representation_build cubesearch.o
	gcc -Irepresentation -O2 brutesolve.c cubesearch.o representation/*.o -o brutesolve -lpthread

facesolve: representation_build cubesearch.o
	gcc -Irepresentation -O2 facesolve.c cubesearch.o representation/*.o -o facesolve -lpthread

indexing_build: representation_build
	cd indexing && make

representation_build:
	cd representation && make

cubesearch.o: cubesearch.c representation_build
	gcc -O2 -c cubesearch.c -Irepresentation -lpthread

clean:
	rm *.o
	rm brutesolve
	rm facesolve
	cd representation && make clean
	cd indexing && make clean
