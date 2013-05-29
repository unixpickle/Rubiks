all: pltools.o cmdutil.o permroot

pltools.o:
	gcc -O2 -c pltools.c -I../

cmdutil.o:
	gcc -O2 -c cmdutil.c -I../

permroot:
	gcc permroot.c -o permroot

clean:
	rm *.o
	rm permroot
