CC = gcc
CFLAGS = -Wall -Iinclude -Iinclude/jansson

LIB_DIR = lib/jansson
INCLUDE_DIR = include

all: alertify

alertify: src/alertify.c
	$(CC) $(CFLAGS) -o alertify src/alertify.c -L$(LIB_DIR) -ljansson

clean:
	rm -f alertify

install: alertify
	sudo cp alertify /usr/local/bin/

uninstall:
	sudo rm -f /usr/local/bin/alertify
