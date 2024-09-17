CC = gcc
CFLAGS = -Wall -Iinclude -Iinclude/jansson
LIBS = -ljansson -lm

LIB_DIR = lib/jansson

all: alertify

alertify: src/alertify.c
	$(CC) $(CFLAGS) -o alertify src/alertify.c -L$(LIB_DIR) $(LIBS)

system:
	sudo mv ./alertify /usr/bin/alertify 
	rm -f alertify

install: alertify
	sudo cp alertify /usr/local/bin/

uninstall:
	sudo rm -f /usr/local/bin/alertify
