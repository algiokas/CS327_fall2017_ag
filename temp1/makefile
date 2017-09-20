CC = gcc
CFLAGS = -g -Wall -ggdb 
OBJECTS = dungeon.o pqueue.o

dungeon : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o dungeon

%.o : %.c
	$(CC) $(CFLAGS) -c $<
