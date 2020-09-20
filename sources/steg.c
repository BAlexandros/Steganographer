#include "../headers/steg.h"

void process_arguments(int argument_num)
{
  if ( argument_num == 1){
    show_help_message();
    exit(-1);
  }
  return;
}

void show_help_message(void)
{
  printf(BOLD_ON "\nSteganography encryptor-decryptor\n\n" BOLD_OFF
         BOLD_ON "Available flags:\n" BOLD_OFF
         BOLD_ON "-e, --encrypt\n" BOLD_OFF
         "    takes two arguments, " UNDERSCORE_ON "file1.png" UNDERSCORE_OFF ", " UNDERSCORE_ON "file2.png" UNDERSCORE_OFF ", and hides file2 inside file1\n"
         "    example: ./steganize -e file1.png file2.png\n"
         BOLD_ON "-d, --decrypt\n" BOLD_OFF
         "    takes one argument, " UNDERSCORE_ON "file.png" UNDERSCORE_OFF ", and attempts to extract the image hidden inside it\n"
         "    example: ./steganize -d file.png\n"
         BOLD_ON "-h, --help\n" BOLD_OFF
         "    show this message\n\n");
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

int hide_png_in_png( char* cover_file_name, char* secret_file_name)
{
  printf("Opening the files and checking their type\n");
  // Open the cover image, in which we will hide the secret image,
  // and verify that it was opened correctly and is a png file
  FILE *cover_image_fp = fopen(cover_file_name,"rb");
  verify_file_is_open(cover_image_fp);
  verify_file_is_png(cover_image_fp);

  // Repeat the opening and verification process for the secret image
  FILE *secret_image_fp = fopen(secret_file_name,"rb");
  verify_file_is_open(secret_image_fp);
  verify_file_is_png(secret_image_fp);

  // Initialize the structures required by libpng to read the cover file */
  png_structp png_ptr_cover   = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
  png_infop   info_ptr_cover  = png_create_info_struct(png_ptr_cover); 
  if (!info_ptr_cover){ 
    png_destroy_read_struct(&png_ptr_cover, (png_infopp)NULL, (png_infopp)NULL); 
    return -1; 
  } 
  png_infop   end_info_cover  = png_create_info_struct(png_ptr_cover); 
  if (!end_info_cover){ 
    png_destroy_read_struct(&png_ptr_cover, &info_ptr_cover, (png_infopp)NULL); 
    return -1; 
  } 
   
  // And the structures for the secret file */
  png_structp png_ptr_secret  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
  png_infop   info_ptr_secret = png_create_info_struct(png_ptr_secret); 
  if (!info_ptr_secret){ 
    png_destroy_read_struct(&png_ptr_secret, NULL, NULL); 
    return -1; 
  } 
  png_infop   end_info_secret  = png_create_info_struct(png_ptr_secret); 
  if (!end_info_secret){ 
    png_destroy_read_struct(&png_ptr_secret, &info_ptr_secret, (png_infopp)NULL); 
    return -1; 
  } 
   
  // Set up input code */
  png_init_io( png_ptr_cover,  cover_image_fp); 
  png_init_io( png_ptr_secret, secret_image_fp); 
  // Inform libpng that bytes are missing from start of files (read to check headers) */
  png_set_sig_bytes( png_ptr_cover,  PNG_HEADER_SIZE);
  png_set_sig_bytes( png_ptr_secret, PNG_HEADER_SIZE);

  /***********************************
   *  Read the files (data and info  *
   **********************************/
  
  // Finally read the info  */
  png_read_info(png_ptr_cover,  info_ptr_cover);
  png_read_info(png_ptr_secret, info_ptr_secret);

  png_uint_32 cov_width, cov_height;
  png_uint_32 sec_width, sec_height;
  int cov_bit_depth, cov_color_type, cov_channels;
  int sec_bit_depth, sec_color_type, sec_channels;

  png_get_IHDR(png_ptr_cover, info_ptr_cover, &cov_width, &cov_height, \
               &cov_bit_depth, &cov_color_type, NULL, NULL, NULL);
  png_get_IHDR(png_ptr_secret, info_ptr_secret, &sec_width, &sec_height, \
               &sec_bit_depth, &sec_color_type, NULL, NULL, NULL);

  // If the colour type is palette indices, change to RGB
  if ( cov_color_type == PNG_COLOR_TYPE_PALETTE){
    png_set_palette_to_rgb( png_ptr_cover);
    png_read_update_info( png_ptr_cover, info_ptr_cover);
    png_get_IHDR(png_ptr_cover, info_ptr_cover, &cov_width, &cov_height, \
                 &cov_bit_depth, &cov_color_type, NULL, NULL, NULL);
    printf("Cover image is using palette indices. Converting to RGB\n");
  }
  if ( sec_color_type == PNG_COLOR_TYPE_PALETTE){
    png_set_palette_to_rgb( png_ptr_secret);
    png_read_update_info( png_ptr_secret, info_ptr_secret);
    png_get_IHDR(png_ptr_secret, info_ptr_secret, &sec_width, &sec_height, \
                 &sec_bit_depth, &sec_color_type, NULL, NULL, NULL);
    printf("Secret image is using palette indices. Converting to RGB\n");
  }

  // If the colour type is grayscale, change to RGB
  if ( cov_color_type == PNG_COLOR_TYPE_GRAY){
    png_set_gray_to_rgb( png_ptr_cover);
    png_read_update_info( png_ptr_cover, info_ptr_cover);
    png_get_IHDR(png_ptr_cover, info_ptr_cover, &cov_width, &cov_height, \
                 &cov_bit_depth, &cov_color_type, NULL, NULL, NULL);
    printf("Cover image is using palette indices. Converting to RGB\n");
  }
  if ( sec_color_type == PNG_COLOR_TYPE_GRAY){
    png_set_gray_to_rgb( png_ptr_secret);
    png_read_update_info( png_ptr_secret, info_ptr_secret);
    png_get_IHDR(png_ptr_secret, info_ptr_secret, &sec_width, &sec_height, \
                 &sec_bit_depth, &sec_color_type, NULL, NULL, NULL);
    printf("Secret image is using palette indices. Converting to RGB\n");
  }

  // If the colour type is RGBA, make it RGB
  if ( cov_color_type & PNG_COLOR_MASK_ALPHA){
    png_set_strip_alpha(png_ptr_cover);
    png_read_update_info( png_ptr_cover, info_ptr_cover);
    png_get_IHDR(png_ptr_cover, info_ptr_cover, &cov_width, &cov_height, \
                 &cov_bit_depth, &cov_color_type, NULL, NULL, NULL);
    printf("Cover image is RGBA. Converting to RGB\n");
  }
  if ( sec_color_type & PNG_COLOR_MASK_ALPHA){
    png_set_strip_alpha(png_ptr_secret);
    png_read_update_info( png_ptr_secret, info_ptr_secret);
    png_get_IHDR(png_ptr_secret, info_ptr_secret, &sec_width, &sec_height, \
                 &sec_bit_depth, &sec_color_type, NULL, NULL, NULL);
    printf("Secret image is using palette indices. Converting to RGB\n");
  }

  cov_channels = png_get_channels( png_ptr_cover,  info_ptr_cover);
  sec_channels = png_get_channels( png_ptr_secret, info_ptr_secret);

  printf("Size of cover  (%s): %u x %u. Bit depth: %d. Color : %d, Channels: %d\n", \
         cover_file_name,  cov_width, cov_height, cov_bit_depth, cov_color_type, cov_channels);
  printf("Size of secret (%s): %u x %u. Bit depth: %d. Color : %d, Channels: %d\n", \
         secret_file_name, sec_width, sec_height, sec_bit_depth, sec_color_type, sec_channels);

  // Exit if the secret does not fit inside the cover
  if ( sec_width*sec_height*sec_channels > cov_width*cov_height*cov_channels){
    printf("The secret image is too large for the cover. Exiting\n");
    return -1;
  }
   
  // Read the images' data
  png_uint_32 cov_rowbytes = png_get_rowbytes( png_ptr_cover,  info_ptr_cover);
  png_uint_32 sec_rowbytes = png_get_rowbytes( png_ptr_secret, info_ptr_secret);
  png_bytep* row_pointers_cover  = malloc( sizeof(png_bytep) * cov_height);
  png_bytep* row_pointers_secret = malloc( sizeof(png_bytep) * sec_height);

  for (unsigned int r = 0; r < cov_height; r++){
    row_pointers_cover[r]  = malloc( cov_rowbytes );
  }
  for (unsigned int r = 0; r < sec_height; r++){
    row_pointers_secret[r] = malloc( sec_rowbytes );
  }
  png_read_image(png_ptr_cover,  row_pointers_cover);
  png_read_image(png_ptr_secret, row_pointers_secret);
  
  printf("Reading of cover and secret images complete\n");

  /**********************
   *  Process the data  *
   *********************/
  printf("Beginning the processing of the data\n");

  // Processing code here
  for ( unsigned int r = 0; r < cov_height; r++){
    for ( unsigned int c = 0; c < cov_rowbytes; c++){
      if ( (r >= sec_height) || (c >= sec_rowbytes)){
        row_pointers_cover[r][c] = ( row_pointers_cover[r][c] & ~0xff) | ( 0xffff & 0xff);
      } 
      else {
        row_pointers_cover[r][c] = ( row_pointers_cover[r][c] & ~0xff) | ( row_pointers_secret[r][c] & 0xff);
      }
    }
  }
  
  printf("Data processed\n");

  
  /***********************
   *  Write output file  *
   **********************/

  printf("Opening output file to store combined image\n");
  
  // Set up output file and its information
  png_uint_32 out_width       = cov_width;
  png_uint_32 out_height      = cov_height;
  int out_bit_depth           = cov_bit_depth;
  int out_color_type          = cov_color_type;
  png_uint_32 out_rowbytes    = cov_rowbytes;
  FILE* out_image_fp = fopen("out.png","wb");
  verify_file_is_open(out_image_fp);
  png_structp png_ptr_out = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr_out  = png_create_info_struct(png_ptr_out);
  /* setjmp(png_jmpbuf(png_ptr_out)); */
  png_init_io(png_ptr_out, out_image_fp);
  png_set_IHDR( png_ptr_out, info_ptr_out, out_width, out_height, out_bit_depth, 
                out_color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, 
                PNG_FILTER_TYPE_BASE);
  png_write_info( png_ptr_out, info_ptr_out);

  // Fill the data with the new image
  png_bytep row_pointers_out = malloc( out_rowbytes );

  if (cov_bit_depth == 8){
    for ( unsigned int y = 0; y < out_height; y++) {
      for ( unsigned int x = 0; x < out_rowbytes; x++) {
          row_pointers_out[x] = row_pointers_cover[y][x];
      }
      png_write_row( png_ptr_out, row_pointers_out);
    }
  }
  png_write_end(png_ptr_out, info_ptr_out);

  printf("Output file written sucessfully\n");

  //Close the file and clean up memory
  fclose(cover_image_fp);
  fclose(secret_image_fp);
  fclose(out_image_fp);

  for (unsigned int i = 0; i < cov_height; i++)
    free(row_pointers_cover[i]);
  free(row_pointers_cover);
  for (unsigned int i = 0; i < sec_height; i++)
    free(row_pointers_secret[i]);
  free(row_pointers_secret);
  free(row_pointers_out);

  png_destroy_read_struct( &png_ptr_cover,  &info_ptr_cover,  &end_info_cover);
  png_destroy_read_struct( &png_ptr_secret, &info_ptr_secret, &end_info_secret);

  return 0;
}

int extract_png_from_png(char* combined_file_name)
{

  printf("Opening \"%s\" to reveal hidden file\n", combined_file_name);
  FILE *combined_image_fp = fopen(combined_file_name,"rb");
  verify_file_is_open(combined_image_fp);
  verify_file_is_png(combined_image_fp);
  // Initialize the structures required by libpng to read the cover file */
  png_structp png_ptr_combined   = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
  png_infop   info_ptr_combined  = png_create_info_struct(png_ptr_combined); 
  if (!info_ptr_combined){ 
    png_destroy_read_struct(&png_ptr_combined, (png_infopp)NULL, (png_infopp)NULL); 
    return -1; 
  } 
  png_infop   end_info_combined  = png_create_info_struct(png_ptr_combined); 
  if (!end_info_combined){ 
    png_destroy_read_struct(&png_ptr_combined, &info_ptr_combined, (png_infopp)NULL); 
    return -1; 
  } 
  png_init_io( png_ptr_combined,  combined_image_fp); 
  // Inform libpng that bytes are missing from start of files (read to check headers) */
  png_set_sig_bytes( png_ptr_combined,  PNG_HEADER_SIZE);

  png_read_info(png_ptr_combined,  info_ptr_combined);

  png_uint_32 comb_width, comb_height;
  int comb_bit_depth, comb_color_type, comb_channels;

  png_get_IHDR(png_ptr_combined, info_ptr_combined, &comb_width, &comb_height, \
               &comb_bit_depth, &comb_color_type, NULL, NULL, NULL);
  comb_channels = png_get_channels( png_ptr_combined,  info_ptr_combined);

  printf("Size of \"%s\": %u x %u. Bit depth: %d. Color : %d, Channels: %d\n", \
         combined_file_name, comb_width, comb_height, comb_bit_depth, 
         comb_color_type, comb_channels);

  // Read the images' data
  png_uint_32 comb_rowbytes = png_get_rowbytes( png_ptr_combined,  info_ptr_combined);
  png_bytep* row_pointers_combined  = malloc( sizeof(png_bytep) * comb_height);

  for (unsigned int r = 0; r < comb_height; r++){
    row_pointers_combined[r]  = malloc( comb_rowbytes );
  }
  png_read_image(png_ptr_combined,  row_pointers_combined);
  
  printf("Reading of combined image complete\n");

  // Set up output file and its information
  printf("Opening file to store hidden image\n");
  png_uint_32 hid_width       = comb_width;
  png_uint_32 hid_height      = comb_height;
  int hid_bit_depth           = comb_bit_depth;
  int hid_color_type          = comb_color_type;
  png_uint_32 hid_rowbytes    = comb_rowbytes;
  FILE* hidden_image_fp = fopen("hidden.png","wb");
  verify_file_is_open(hidden_image_fp);
  png_structp png_ptr_hidden = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr_hidden  = png_create_info_struct(png_ptr_hidden);
  png_init_io(png_ptr_hidden, hidden_image_fp);
  png_set_IHDR( png_ptr_hidden, info_ptr_hidden, hid_width, hid_height, hid_bit_depth, 
                hid_color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, 
                PNG_FILTER_TYPE_BASE);
  png_write_info( png_ptr_hidden, info_ptr_hidden);

  // Fill the data with the new image
  png_bytep row_pointers_hidden = malloc( hid_rowbytes );

  if (hid_bit_depth == 8){
    for ( unsigned int y = 0; y < hid_height; y++) {
      for ( unsigned int x = 0; x < hid_rowbytes; x++) {
        if ( (row_pointers_combined[y][x] & 15) == 15 ){
          row_pointers_hidden[x] = 255;
        } 
        else {
          row_pointers_hidden[x] = row_pointers_combined[y][x] << 4;
        }
      }
      png_write_row( png_ptr_hidden, row_pointers_hidden);
    }
  }
  png_write_end(png_ptr_hidden, info_ptr_hidden);

  printf("Data written to hidden.png\n");

  fclose(combined_image_fp);
  fclose(hidden_image_fp);

  for (unsigned int i = 0; i < comb_height; i++)
    free(row_pointers_combined[i]);
  free(row_pointers_combined);
  free(row_pointers_hidden);

  return 0;
}

