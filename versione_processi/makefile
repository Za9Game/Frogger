final: main.o player.o render.o menu.o crocodile.o bullet.o bush.o collisions.o timer.o
	gcc main.o player.o render.o menu.o crocodile.o bullet.o bush.o collisions.o timer.o -lncursesw -o main -I.
	rm -f *.o

main.o: main.c
	gcc -lncursesw -c main.c

render.o: render.c
	gcc -lncursesw -c render.c

menu.o: menu.c
	gcc -lncursesw -c menu.c

player.o: player.c
	gcc -lncursesw -c player.c

crocodile.o: crocodile.c
	gcc -lncursesw -c crocodile.c

bullet.o: bullet.c
	gcc -lncursesw -c bullet.c

bush.o: bush.c
	gcc -lncursesw -c bush.c

collisions.o: collisions.c
	gcc -lncursesw -c collisions.c

timer.o: timer.c
	gcc -lncursesw -c timer.c
