#include <stdio.h>
#include <stdlib.h>
#include <png.h>

#define PNG_HEADER_SIZE 8

/*************************
 *  Function prototypes  *
 ************************/

void verify_minimum_arguments(int);
void verify_file_is_open(FILE*);
void verify_file_is_png(FILE*);

/************************/

int main(int argc, char *argv[])
{
  //
  // Check if enough arguments were given, and give a help message if not
  verify_minimum_arguments(argc);

  // Open the cover image, in which we will hide the secret image,
  // and verify that it was opened correctly and is a png file
  char* cover_filename = argv[1];
  FILE *cover_image_fp = fopen(cover_filename,"rb");
  verify_file_is_open(cover_image_fp);
  verify_file_is_png(cover_image_fp);

  // Repeat the opening and verification process for the secret image
  char* secret_filename = argv[2];
  FILE *secret_image_fp = fopen(secret_filename,"rb");
  verify_file_is_open(secret_image_fp);
  verify_file_is_png(secret_image_fp);


  // Close the files after processing them
  fclose(cover_image_fp);
  fclose(secret_image_fp);

  return 0;
}

/****************************
 *   Function definitions   *
 ***************************/

void verify_minimum_arguments(int argument_num)
{
  if ( argument_num < 3){
    printf("You have not entered enough arguments. \n"
           "Try entering the cover image as the first argument, and the secret image as the second argument. \n"
           "(Note: Only PNG files are currently supported)\n");
    exit(-1);
  }
  return;
}

void verify_file_is_open(FILE* filename)
{
  if (!filename){
    printf("Error: Problem opening files. Exiting\n");
    exit(-1);
  }
  return;
}

void verify_file_is_png(FILE* filename)
{
  // Read the header and terminate the program if the file is not a png
  unsigned char header[PNG_HEADER_SIZE];
  fread(header, 1, PNG_HEADER_SIZE, filename);
  if (png_sig_cmp(header, 0, PNG_HEADER_SIZE)){
    printf("Error: Currently only png files are supported. Exiting\n");
    exit(-1);
  }
  return;
}
