main: indexmaker

indexmaker: configcounter.o
	gcc configcounter.o ../representation/cube.o indexmaker.c -o indexmaker -I../representation

configcounter.o: configcounter.c shard.o index.o
	gcc -c configcounter.c -I../representation -o _configcounter.o
	ld -r shard.o index.o _configcounter.o -o configcounter.o
	rm _configcounter.o

shard.o: shard.c
	gcc -c shard.c -I../representation

index.o: index.c
	gcc -c index.c -I../representation

clean:
	rm *.o
	rm indexmaker
