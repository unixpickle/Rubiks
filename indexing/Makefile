main: indexmaker

indexmaker: configcounter.o
	gcc configcounter.o ../representation/cube.o indexmaker.c -o indexmaker -I../representation

configcounter.o: configcounter.c index.o
	gcc -c configcounter.c -I../representation -o _configcounter.o
	ld -r index.o _configcounter.o -o configcounter.o
	rm _configcounter.o

shard.o: shard.c
	gcc -c shard.c -I../representation

index.o: index.c shard.o
	gcc -c index.c -I../representation -o _index.o
	ld -r _index.o shard.o -o index.o
	rm _index.o
	

clean:
	rm *.o
	rm indexmaker
