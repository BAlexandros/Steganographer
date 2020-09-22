#include "../headers/steg.h"

void init_configs(struct config_struct* configurations)
{
  configurations->cover_file_name     = NULL;
  configurations->secret_file_name    = NULL;
  configurations->output_file_name    = "out.png";
  configurations->combined_file_name  = NULL;
  configurations->encrypt_flag    = 0;
  configurations->decrypt_flag    = 0;
  configurations->out_name_flag   = 0;
  configurations->sig_bits        = 4;
  configurations->brighten_cover  = 0;
  configurations->padding_white   = 0;

  return;
}

void process_arguments(int argument_num, char* argv[], struct config_struct* configurations)
{
  if ( argument_num == 1){
    show_help_message();
    exit(-1);
  }
  int i = 1;
  while (i < argument_num){

    if (!strcmp( argv[i], "-h") || !(strcmp( argv[i], "--help")))
      show_help_message();

    if (!strcmp( argv[i], "-e") || !(strcmp( argv[i], "--encrypt"))){
      configurations->encrypt_flag     = 1;
      if (argv[i+1] == NULL || argv[i+2] == NULL || argv[i+1][0] == '-' || argv[i+2][0] == '-'){
        printf("Invalid syntax. Please use -h or --help for further information\n");
        exit(-1);
      }
      configurations->cover_file_name  = argv[i+1];
      configurations->secret_file_name = argv[i+2];
      i+=3;
      continue;
    }

    if (!strcmp( argv[i], "-d") || !(strcmp( argv[i], "--decrypt"))){
      configurations->decrypt_flag = 1;
      if (argv[i+1] == NULL || argv[i+1][0] == '-'){
        printf("Invalid syntax. Please use -h or --help for further information\n");
        exit(-1);
      }
      configurations->combined_file_name  = argv[i+1];
      i+=2;
      continue;
    }

    if (!strcmp( argv[i], "-o") || !(strcmp( argv[i], "--output_name"))){
      configurations->out_name_flag = 1;
      if (argv[i+1] == NULL || argv[i+1][0] == '-'){
        printf("Invalid syntax. Please use -h or --help for further information\n");
        exit(-1);
      }
      configurations->output_file_name = argv[i+1];
      i+=2;
      continue;
    }

    if (!strcmp( argv[i], "-s") || !(strcmp( argv[i], "--sigs"))){
      if ( atoi(argv[i+1]) <= 0){
        printf("The number of bits to be replaced must be positive. Using default value\n");
      } else {
        configurations->sig_bits = atoi(argv[i+1]);
      }
      i+=2;
      continue;
    }

    if (!strcmp( argv[i], "-b") || !(strcmp( argv[i], "--brighten_cover"))){
      configurations->brighten_cover = 1;
      i++;
      continue;
    }

    if (!strcmp( argv[i], "-p") || !(strcmp( argv[i], "--padding_white"))){
      configurations->padding_white = 1;
      i++;
      continue;
    }
    i++;
  }

  // Make sure we are either encrypting or decrypting. No need to do both in one call
  if ( configurations->encrypt_flag == configurations->decrypt_flag){
    printf("Please pick either to encrypt or to decrypt in each command call.\n"
           "You can use -h or --help for more information.\n");
    exit(-1);
  }

  return;
}

void show_help_message(void)
{
  printf(BOLD_ON "\nSteganography encryptor-decryptor\n\n" BOLD_OFF
         BOLD_ON "SYNOPSIS\n" BOLD_OFF
         "    This program can be used to hide an image inside an image. The terms \"encrypt\" and\n"
         "    \"decrypt\" are used, but there is no actual cryptography involved. Information\n"
         "    is hidden within plain sight, by replacing the least significant bits of the cover\n"
         "    image with the most significant of the secret image. We can then recover the secret\n"
         "    by analyzing the combined image. Of course, information is lost in the process depending\n"
         "    on the number of bits used in the encrypting process. Still, a balance must be kept.\n"
         "    Replacing too many bits of the cover image will make the secret too visible, and keeping\n"
         "    too few of the secret will make the quality much lower when recovered\n\n"
         BOLD_ON "FLAGS:\n" BOLD_OFF
         BOLD_ON "    -e, --encrypt  " BOLD_OFF ITALICS_ON "file1.png" ITALICS_OFF " " ITALICS_ON "file2.png" ITALICS_OFF "\n"
         "        takes two arguments, file1.png, file2.png and hides file2 inside file1\n"
         "        example: ./steganize -e file1.png file2.png\n"
         BOLD_ON "    -d, --decrypt  " BOLD_OFF ITALICS_ON "file.png" ITALICS_OFF "\n"
         "        takes one argument, file.png, and attempts to extract the image hidden inside it\n"
         "        example: ./steganize -d file.png\n"
         BOLD_ON "    -s, --sig      " BOLD_OFF ITALICS_ON "N" ITALICS_OFF "=4    (" UNDERSCORE_ON "OPTIONAL FLAG" UNDERSCORE_OFF ")\n"
         "        takes one argument, N, which determined the number of Least Significant Bits of the cover\n"
         "        image to replace with Most Significant Bits of the secret image. Default value is 4.\n"
         BOLD_ON "    -b, --brighten        (" BOLD_OFF UNDERSCORE_ON "OPTIONAL FLAG" UNDERSCORE_OFF ")\n"
         "        does not require any arguments. If the flag is not present, then the pixels which\n"
         "        are not overlaping with the secret will have their LSBs replaced by 0 (black). If the flag is\n"
         "        present, then the LSBs will be set to 1 (white). This is useful to determine which pixels belong\n"
         "        to the cover and which to the secret when decoding. Default color is black.\n"
         BOLD_ON "    -p, --padding         (" BOLD_OFF UNDERSCORE_ON "OPTIONAL FLAG" UNDERSCORE_OFF ")\n"
         "        does not require any arguments. If the flag is not present, then the pixels which\n"
         "        are determined not to be part of the hidden image will be switched to black.\n"
         "        If the flag is present, they will be switched to white. Default color is black.\n"
         BOLD_ON "    -h, --help\n" BOLD_OFF
         "        show this message\n\n"
         BOLD_ON "LIMITATIONS\n" BOLD_OFF
         "    Currently only RGB PNG files with 8 bits per channel are supported. Non-RGB files will\n"
         "    be converted to RGB either by expanding if there are less channels, or by stripping any\n"
         "    extra ( channels like alpha are therefore lost). I hope to eventually manage to further\n"
         "    the support for PNGs, then maybe even more types of files.\n"
         );

  exit(-1);
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

int hide_png_in_png(struct config_struct confs)
{
  printf("Opening the files and checking their type\n");
  // Open the cover image, in which we will hide the secret image,
  // and verify that it was opened correctly and is a png file
  FILE *cover_image_fp = fopen(confs.cover_file_name,"rb");
  verify_file_is_open(cover_image_fp);
  verify_file_is_png(cover_image_fp);

  // Repeat the opening and verification process for the secret image
  FILE *secret_image_fp = fopen(confs.secret_file_name,"rb");
  verify_file_is_open(secret_image_fp);
  verify_file_is_png(secret_image_fp);

  // Initialize the structures required by libpng to read the cover file */
  png_structp png_ptr_cover   = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
  png_infop   info_ptr_cover  = png_create_info_struct(png_ptr_cover); 
  if (!info_ptr_cover){ 
    png_destroy_read_struct(&png_ptr_cover, (png_infopp)NULL, (png_infopp)NULL); 
    exit(-1);
  } 
  png_infop   end_info_cover  = png_create_info_struct(png_ptr_cover); 
  if (!end_info_cover){ 
    png_destroy_read_struct(&png_ptr_cover, &info_ptr_cover, (png_infopp)NULL); 
    exit(-1);
  } 
   
  // And the structures for the secret file */
  png_structp png_ptr_secret  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
  png_infop   info_ptr_secret = png_create_info_struct(png_ptr_secret); 
  if (!info_ptr_secret){ 
    png_destroy_read_struct(&png_ptr_secret, NULL, NULL); 
    exit(-1);
  } 
  png_infop   end_info_secret  = png_create_info_struct(png_ptr_secret); 
  if (!end_info_secret){ 
    png_destroy_read_struct(&png_ptr_secret, &info_ptr_secret, (png_infopp)NULL); 
    exit(-1);
  } 
   
  // Set up input code */
  png_init_io( png_ptr_cover,  cover_image_fp); 
  png_init_io( png_ptr_secret, secret_image_fp); 
  // Inform libpng that bytes are missing from start of files (read to check headers) */
  png_set_sig_bytes( png_ptr_cover,  PNG_HEADER_SIZE);
  png_set_sig_bytes( png_ptr_secret, PNG_HEADER_SIZE);

  /***********************************
   *  Read the files (data and info) *
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
    // 
    png_set_strip_alpha(png_ptr_secret);
    png_read_update_info( png_ptr_secret, info_ptr_secret);
    png_get_IHDR(png_ptr_secret, info_ptr_secret, &sec_width, &sec_height, \
                 &sec_bit_depth, &sec_color_type, NULL, NULL, NULL);
    printf("Secret image is using palette indices. Converting to RGB\n");
  }

  cov_channels = png_get_channels( png_ptr_cover,  info_ptr_cover);
  sec_channels = png_get_channels( png_ptr_secret, info_ptr_secret);

  printf("Size of cover  (%s): %u x %u. Bit depth: %d. Color : %d, Channels: %d\n", \
         confs.cover_file_name,  cov_width, cov_height, cov_bit_depth, cov_color_type, cov_channels);
  printf("Size of secret (%s): %u x %u. Bit depth: %d. Color : %d, Channels: %d\n", \
         confs.secret_file_name, sec_width, sec_height, sec_bit_depth, sec_color_type, sec_channels);

  // Exit if the secret does not fit inside the cover
  if ( sec_width*sec_height*sec_channels > cov_width*cov_height*cov_channels){
    printf("The secret image is too large for the cover. Exiting\n");
    exit(-1);
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

  int bitmask = get_mask_from_sig_bits(confs.sig_bits);
  if ( confs.sig_bits > cov_bit_depth || confs.sig_bits > sec_bit_depth){
    printf("The significant bits requested are too many. Defaulting to 4\n");
    confs.sig_bits = 4;
  }

  if ( cov_bit_depth == 8 && sec_bit_depth == 8){
    for ( unsigned int r = 0; r < cov_height; r++){
      for ( unsigned int c = 0; c < cov_rowbytes; c++){
        if ( (r >= sec_height) || (c >= sec_rowbytes)){
          row_pointers_cover[r][c] = ( row_pointers_cover[r][c] & ~bitmask) | ( (confs.brighten_cover * 255) & bitmask);
        } 
        else {
          row_pointers_cover[r][c] = ( row_pointers_cover[r][c] & ~bitmask) | ( (row_pointers_secret[r][c]>>(cov_bit_depth-confs.sig_bits)) & bitmask);
        }
      }
    }
  }
  else {
    printf("No support for 16 bit channels yet\n");
    exit(-1);
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
  FILE* out_image_fp = fopen(confs.output_file_name,"wb");
  verify_file_is_open(out_image_fp);
  png_structp png_ptr_out = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr_out  = png_create_info_struct(png_ptr_out);
  /* setjmp(png_jmpbuf(png_ptr_out)); */
  png_init_io(png_ptr_out, out_image_fp);
  png_set_IHDR( png_ptr_out, info_ptr_out, out_width, out_height, out_bit_depth, 
                out_color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, 
                PNG_FILTER_TYPE_DEFAULT);
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

  printf("Combined images written sucessfully in %s\n", confs.output_file_name);

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

int extract_png_from_png(struct config_struct confs)
{

  printf("Opening \"%s\" to reveal hidden file\n", confs.combined_file_name);
  FILE *combined_image_fp = fopen(confs.combined_file_name,"rb");
  verify_file_is_open(combined_image_fp);
  verify_file_is_png(combined_image_fp);
  // Initialize the structures required by libpng to read the cover file */
  png_structp png_ptr_combined   = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
  png_infop   info_ptr_combined  = png_create_info_struct(png_ptr_combined); 
  if (!info_ptr_combined){ 
    png_destroy_read_struct(&png_ptr_combined, (png_infopp)NULL, (png_infopp)NULL); 
    exit(-1);
  } 
  png_infop   end_info_combined  = png_create_info_struct(png_ptr_combined); 
  if (!end_info_combined){ 
    png_destroy_read_struct(&png_ptr_combined, &info_ptr_combined, (png_infopp)NULL); 
    exit(-1);
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
         confs.combined_file_name, comb_width, comb_height, comb_bit_depth, 
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
  FILE* hidden_image_fp = fopen(confs.output_file_name,"wb");
  verify_file_is_open(hidden_image_fp);
  png_structp png_ptr_hidden = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr_hidden  = png_create_info_struct(png_ptr_hidden);
  png_init_io(png_ptr_hidden, hidden_image_fp);
  png_set_IHDR( png_ptr_hidden, info_ptr_hidden, hid_width, hid_height, hid_bit_depth, 
                hid_color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, 
                PNG_FILTER_TYPE_DEFAULT);
  png_write_info( png_ptr_hidden, info_ptr_hidden);

  // Fill the data with the new image
  png_bytep row_pointers_hidden = malloc( hid_rowbytes );

  if (hid_bit_depth == 8){

    int bitmask = get_mask_from_sig_bits(confs.sig_bits); 

    if ( confs.sig_bits > hid_bit_depth){
      printf("The significant bits you requested are too many. Defaulting to 4\n");
      confs.sig_bits = 4;
    }

    for ( unsigned int y = 0; y < hid_height; y++) {
      for ( unsigned int x = 0; x < hid_rowbytes; x++) {
        if ( (row_pointers_combined[y][x] & bitmask) == ((confs.brighten_cover) & bitmask)){
          row_pointers_hidden[x] = confs.padding_white*255;
        } 
        else {
          row_pointers_hidden[x] = row_pointers_combined[y][x] << (hid_bit_depth-confs.sig_bits);
        }
      }
      png_write_row( png_ptr_hidden, row_pointers_hidden);
    }
  }
  png_write_end(png_ptr_hidden, info_ptr_hidden);

  printf("Data written to %s\n", confs.output_file_name);

  fclose(combined_image_fp);
  fclose(hidden_image_fp);

  for (unsigned int i = 0; i < comb_height; i++)
    free(row_pointers_combined[i]);
  free(row_pointers_combined);
  free(row_pointers_hidden);

  return 0;
}

int get_mask_from_sig_bits(int sigs)
{
  int sum = 0;
  for (int i = 0; i < sigs; i++){
    sum += pow(2,i);
  }
  return sum;
}

