CC := gcc
CFLAGS := -std=c99 -ggdb -Wall
IFLAGS := -Iinclude/ -Isrc/ -Ilibs/rebound/
LFLAGS := -lm libs/rebound/rebound.o

SRC := $(wildcard src/*.c)
MODULES := $(subst .c,.imod,$(addprefix bin/,$(wildcard modules/*.c)))

iris: libs mods
	@mkdir -p bin/
	$(CC) $(CFLAGS) $(SRC) -o bin/iris $(IFLAGS) $(LFLAGS)

libs: libs/rebound/rebound.o

libs/rebound/rebound.o: libs/rebound/rebound.c
	$(CC) $(CFLAGS) -fPIC -c libs/rebound/rebound.c -o libs/rebound/rebound.o -Ilibs/rebound/ -lm

mods: $(MODULES)

bin/modules/%.imod: modules/%.c
	@mkdir -p bin/modules/
	$(CC) $(CFLAGS) -shared $< -o $@ $(IFLAGS) $(LFLAGS)
