CC = gcc
CFLAGS = -g -Wall -Werror -ggdb -O2
OBJECTS = dungeon.o pqueue.o main.o

dungeon : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o dungeon

%.o : %.c
	$(CC) $(CFLAGS) -c $<

.PHONY : clean

clean :
	rm dungeon $(OBJECTS)
