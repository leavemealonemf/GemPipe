CC = gcc
CFLAGS = -Wall -I.
LDFLAGS = -L. -lcjson
OUTPUT = gempipe

all: gempipe

gempipe: main.c
	$(CC) $(CFLAGS) -o $(OUTPUT) main.c external/libcjson.a

clean:
	rm -f $(OUTPUT)

