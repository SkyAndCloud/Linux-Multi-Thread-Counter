main: main.c
	gcc -std=c99 -D_REENTRANT main.c -o multisum -lpthread
clear:
	rm *.o main
