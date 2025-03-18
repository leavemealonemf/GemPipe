
LIBS = -lcjson
OUTPUT = pipe

add:
	gcc -Wall -o $(OUTPUT) $(LIBS) main.c

clear:
	rm pipe