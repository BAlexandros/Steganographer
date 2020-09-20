#include "../headers/steg.h"

int main(int argc, char *argv[])
{
  char* cover_file_name;    // The file in which the secret will be hidden
  char* secret_file_name;   // The file which will be hiden
  char* combined_file_name; // The cover file with the secret contained
  int   encrypt_flag;       // -e to encrypt 
  int   decrypt_flag;       // -d to decrypt
  int   help_flag;          // -h to show help

  // Check if enough arguments were given, and give a help message if not
  process_arguments(argc);
  cover_file_name  = argv[1];
  secret_file_name = argv[2];

  hide_png_in_png(cover_file_name, secret_file_name);

  extract_png_from_png("out.png");

  return 0;
}

