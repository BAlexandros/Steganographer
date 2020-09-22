#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

#define PNG_HEADER_SIZE 8

#define BOLD_ON          "\033[1m"
#define UNDERSCORE_ON    "\033[4m"
#define BOLD_OFF         "\033[0m"
#define UNDERSCORE_OFF   "\033[0m"

struct config_struct
{
  char* cover_file_name;    // The file in which the secret will be hidden
  char* secret_file_name;   // The file which will be hiden
  char* combined_file_name; // The cover file with the secret contained
  char* output_file_name;   // The name of the file that will be outputed
  int   encrypt_flag;       // -e to encrypt 
  int   decrypt_flag;       // -d to decrypt
  int   out_name_flag;      // -o to name output
  int   sig_bits;           // -s to set the number of bits replaced at each channel
  int   brighten_cover;     // -b to choose whether the padding around the secret will be
                            //    dimmed or brightened
  int   padding_black;      // -p to choose if the padding will be black or whit
};

void init_configs(struct config_struct*);
void process_arguments(int,char* [], struct config_struct*);
void show_help_message(void);

void verify_file_is_open(FILE*);
void verify_file_is_png(FILE*);

int hide_png_in_png(struct config_struct);
int extract_png_from_png(struct config_struct);
