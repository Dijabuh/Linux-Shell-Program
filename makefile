cbrsh: cbrsh.o
	gcc cbrsh.o -o cbrsh -std=c99

cbrsh.o: cbrsh.c
	gcc -c cbrsh.c

clean:
	rm -rf *.o
