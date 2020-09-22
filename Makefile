# Usage: 
# make -> generate "steganize" executable
# make clean -> clean up executable 

CC := gcc
GCCFLAGS := -Wall
LINKERFLAGS := -lpng -lm

all: steganographer

steganographer: sources/main.c sources/steg.c
	@echo "Making \"steganize\" executable in current directory"
	${CC} ${GCCFLAGS} -o steganize ./sources/main.c ./sources/steg.c ${LINKERFLAGS}
	@echo "Done"

clean: steganize
	@echo "Removing executable"
	@rm steganize
	@echo "Executable removed"

