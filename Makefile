
space: function.o giocatore.o nemico.o start.o main.o
	gcc main.o function.o giocatore.o nemico.o start.o -o space.out -lncurses -lpthread 

nemico.o: nemico.c function.h define.h
	gcc -c nemico.c 

giocatore.o: giocatore.c function.h define.h
	gcc -c giocatore.c 

start.o: start.c function.h define.h
	gcc -c start.c 

function.o: function.c function.h define.h
	gcc -c function.c 

main.o: main.c function.h define.h
	gcc -c main.c 

clean: 
	rm space
	rm *.o

