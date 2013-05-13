orientpl_cmd.o: orientpl.o
	gcc -O2 -c orientpl_cmd.c -o _orientpl_cmd.o -I../../
	ld -r _orientpl_cmd.o orientpl.o -o orientpl_cmd.o
	rm _orientpl_cmd.o

orientpl.o:
	gcc -O2 -c orientpl.c -I../../
