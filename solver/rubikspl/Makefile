rubikspl_cmd.o: rubikspl.o
	gcc -O2 -c rubikspl_cmd.c -o _rubikspl_cmd.o -I../../
	ld -r _rubikspl_cmd.o rubikspl.o -o rubikspl_cmd.o
	rm _rubikspl_cmd.o

rubikspl.o:
	gcc -O2 -c rubikspl.c -I../../
