include config.mk

all: libarena.a libarena.so

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

libarena.a: src/arena.o
	ar rcs $@ $^

libarena.so: src/arena.o
	$(CC) -shared -o $@ $^ $(LDFLAGS)

clean:
	rm -f src/*.o libarena.a libarena.so

.PHONY: all clean libarena.a libarena.so
