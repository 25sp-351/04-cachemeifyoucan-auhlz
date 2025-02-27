
OBJS = main.o rodcut.o vec.o cache.o
LIBS = liblast_recently_used.so libleast_recently_used.so libfifo_cache.so

all: main $(LIBS)

CC = gcc
CFLAGS = -Wall -ansi -std=c11 --pedantic -Wextra

# A Rule: How to make a .so from a .c file
lib%.so: %.c cache.h
	$(CC) -shared -fPIC $(CFLAGS) -o $@ $<

main: $(OBJS)
	gcc -o $@ $(CFLAGS) $(OBJS) -ldl

# DEPENDENCIES

main.o: rodcut.h cache.h vec.h

rodcut.o: rodcut.h vec.h

vec.o: vec.h

clean:
	rm -f main $(OBJS) $(LIBS)