crosspl_cmd.o: crosspl.o
	gcc -O2 -c crosspl_cmd.c -o _crosspl_cmd.o -I../../
	ld -r _crosspl_cmd.o crosspl.o -o crosspl_cmd.o
	rm _crosspl_cmd.o

crosspl.o:
	gcc -O2 -c crosspl.c -I../../
