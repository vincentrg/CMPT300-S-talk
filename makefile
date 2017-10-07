main: main.c main.h
	gcc -o s-talk main.c list.o -pthread

clean:
	rm s-talk
