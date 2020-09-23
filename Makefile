# Usage: 
# make 						-> generate "steganize" executable in current directory
# make clean 			-> remove executable from current directory
# make install 		-> generate "steganize" executable in /bin *root permission*
# make uninstall 	-> remove "steganize" executable from /bin *root permission*
# make allclean		-> remove "steganize" executables from current directory and
# 									 /bin, and then erase source files

CC := gcc
GCCFLAGS := -Wall
LINKERFLAGS := -lpng -lm

all: steganographer

steganographer: sources/main.c sources/steg.c
	@echo "Making \"steganize\" executable in current directory"
	${CC} ${GCCFLAGS} -o steganize ./sources/main.c ./sources/steg.c ${LINKERFLAGS}
	@echo "Done"

install: sources/main.c sources/steg.c
	@echo "Installing \"steganize\" executable in /bin directory"
	${CC} ${GCCFLAGS} -o /bin/steganize ./sources/main.c ./sources/steg.c ${LINKERFLAGS}
	@echo "Done"

clean: steganize
	@echo "Removing \"steganize\" executable from current directory"
	@rm steganize
	@echo "Executable removed"

uninstall: /bin/steganize
	@echo "Removing \"steganize\" executable from /bin directory"
	rm /bin/steganize
	@echo "Executable removed"

allclean:
	@echo "All files and executables related to the steganographer program will be removed"
	@echo "If you created an executable using "sudo make install", you might have to run as root to remove it\n"
	@echo "Are you sure you are in the steganographer project's parent directory?"
	@echo "Running this command from somewhere else might cause loss of data"
	@read -p "(y/n): " yn; \
		case $$yn in  \
			y )  \
				if [ -e ./steganize ]; then rm ./steganize; fi; \
				if [ -e /bin/steganize ]; then rm /bin/steganize; fi; \
				rm -rf ./headers; \
				rm -rf ./sources; \
				rm -rf ./.git; \
				rm ./.gitignore; \
				rm README.md; \
				rm ./Makefile; \
				echo "Finished removing"; \
				break;;  \
		 	n )  \
				echo "Nothing happened"; \
				exit;; \
		esac
