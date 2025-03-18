
LIBS = -lcjson
OUTPUT = gempipe

add:
	gcc -Wall -o $(OUTPUT) $(LIBS) main.c

clear:
	rm pipe