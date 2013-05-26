main: indexmaker

indexmaker: configcounter.o
	gcc configcounter.o ../representation/cube.o ../util/cmdutil.o ../util/pltools.o indexmaker.c -o indexmaker -I../

configcounter.o: configcounter.c index.o
	gcc -c configcounter.c -I../ -o _configcounter.o
	ld -r index.o _configcounter.o -o configcounter.o
	rm _configcounter.o

shard.o: shard.c
	gcc -c shard.c -I../

index.o: index.c shard.o mapsymmetry.o
	gcc -c index.c -I../ -o _index.o
	ld -r _index.o shard.o mapsymmetry.o -o index.o
	rm _index.o

mapsymmetry.o:
	gcc -O2 -c mapsymmetry.c

clean:
	rm *.o
	rm indexmaker
