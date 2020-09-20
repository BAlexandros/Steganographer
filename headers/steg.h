#include <stdio.h>
#include <stdlib.h>
#include <png.h>

#define PNG_HEADER_SIZE 8

#define BOLD_ON          "\033[1m"
#define UNDERSCORE_ON    "\033[4m"
#define BOLD_OFF         "\033[0m"
#define UNDERSCORE_OFF   "\033[0m"

void show_help_message(void);
void process_arguments(int);

void verify_file_is_open(FILE*);
void verify_file_is_png(FILE*);

int hide_png_in_png(char*, char*);
int extract_png_from_png(char*);
