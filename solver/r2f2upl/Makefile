r2f2upl_cmd.o: r2f2upl.o
	gcc -O2 -c r2f2upl_cmd.c -o _r2f2upl_cmd.o -I../../
	ld -r _r2f2upl_cmd.o r2f2upl.o -o r2f2upl_cmd.o
	rm _r2f2upl_cmd.o

r2f2upl.o:
	gcc -O2 -c r2f2upl.c -I../../
