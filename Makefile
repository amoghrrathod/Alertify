
CC = gcc
CFLAGS = -Wall
LIBS = $(shell pkg-config --cflags --libs jansson)# Use shell to evaluate pkg-config

# Ensure the file compiles properly and links against jansson
all: alertify

alertify: alertify.c
	$(CC) $(CFLAGS) -o alertify alertify.c $(LIBS)

clean:
	rm -f alertify

install: alertify
	sudo cp alertify /usr/local/bin/

uninstall:
	sudo rm -f /usr/local/bin/alertify

