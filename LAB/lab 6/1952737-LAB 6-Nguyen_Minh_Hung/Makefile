all: mem.o main.o
	gcc -pthread -o mem mem.c main.c
main.o: main.c mem.c
	gcc -pthread -c main.c
mem.o: mem.h
	gcc -pthread -c mem.c
clean:
	rm -f *.o mem