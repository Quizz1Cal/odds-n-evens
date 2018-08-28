# makefile
CC = gcc
CFLAGS = -Wall -g -c -o
DEPS = main.c main.h analytic.c analytic.h user_interface.c user_interface.h game_struct.c game_struct.h
SHARED_DEPS = game_struct.c game_struct.h

game_struct.o: $(SHARED_DEPS)
	$(CC) $(CFLAGS) $@ $<
 
user_interface.o: user_interface.c user_interface.h $(SHARED_DEPS)
	$(CC) $(CFLAGS) $@ $<

analytic.o: analytic.c analytic.h $(SHARED_DEPS)
	$(CC) $(CFLAGS) $@ $<

all: $(DEPS)
	$(CC) $(CFLAGS) game_struct.o game_struct.c
	$(CC) $(CFLAGS) user_interface.o user_interface.c
	$(CC) $(CFLAGS) analytic.o analytic.c
	$(CC) -Wall -g -o main main.c game_struct.o user_interface.o analytic.o
  
main: $(DEPS)
	$(CC) -Wall -g -o $@ main.c game_struct.o user_interface.o analytic.o
 
clean:
	rm -f *.o