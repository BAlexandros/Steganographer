# Usage: 
# make -> generate "stegify" executable
# make clean -> clean up executable 

CC := gcc
LINKERFLAGS := -lm

all: steganographer

steganographer: main.c
	@echo "Making \"stegify\" executable"
	${CC} -o stegify main.c ${LINKERFLAGS}
	@echo "Done"

clean: stegify
	@echo "Removing executable"
	@rm stegify

