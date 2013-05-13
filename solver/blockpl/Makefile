blockpl_cmd.o: blockpl.o
	gcc -O2 -c blockpl_cmd.c -o _blockpl_cmd.o -I../../
	ld -r _blockpl_cmd.o blockpl.o -o blockpl_cmd.o
	rm _blockpl_cmd.o

blockpl.o:
	gcc -O2 -c blockpl.c -I../../
