CC := gcc
CFLAGS := -std=c99 -ggdb
IFLAGS := -Iinclude/ -Isrc/ -Ilibs/rebound/ -Ilibs/glad/include/
LFLAGS := -lm libs/rebound/rebound.o -lglfw libs/glad/glad.o

SRC := $(wildcard src/*.c)
MODULES := $(subst .c,.imod,$(addprefix bin/,$(wildcard modules/*.c)))

iris: libs mods
	@mkdir -p bin/
	cp -r resources bin/
	$(CC) $(CFLAGS) $(SRC) -o bin/iris $(IFLAGS) $(LFLAGS)

libs: libs/rebound/rebound.o

libs/rebound/rebound.o: libs/rebound/rebound.c libs/glad/glad.o
	$(CC) $(CFLAGS) -fPIC -c libs/rebound/rebound.c -o libs/rebound/rebound.o -Ilibs/rebound/ -lm

libs/glad/glad.o: libs/glad/src/gl.c
	$(CC) -O3 -fPIC -c libs/glad/src/gl.c -o libs/glad/glad.o -Ilibs/glad/include

mods: $(MODULES)

bin/modules/%.imod: modules/%.c
	@mkdir -p bin/modules/
	$(CC) $(CFLAGS) -shared $< -o $@ $(IFLAGS) $(LFLAGS)
