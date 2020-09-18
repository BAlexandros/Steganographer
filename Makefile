# Usage: 
# make -> generate "steganize" executable
# make clean -> clean up executable 

CC := gcc
GCCFLAGS := -Wall
LINKERFLAGS := -lpng

all: steganographer

steganographer: main.c
	@echo "Making \"steganize\" executable in current directory"
	${CC} ${GCCFLAGS} -o steganize main.c ${LINKERFLAGS}
	@echo "Done"

clean: steganize
	@echo "Removing executable"
	@rm steganize
	@echo "Executable removed"

