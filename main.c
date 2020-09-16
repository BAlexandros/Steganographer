#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "headers/stb_image.h"
#include "headers/stb_image_write.h"

#define DESIRED_OUT_QUALITY_PERC 100
#define HIDE_MODE   1
#define REVEAL_MODE 1

int main(void)
{

  if (HIDE_MODE){

    // Cover image information
    unsigned int cover_width, cover_height, cover_channels;
    unsigned char *cover_data = stbi_load("./test_images/sample1.jpeg", &cover_width, &cover_height, &cover_channels, 0);
    if ( cover_data == NULL || cover_width*cover_height*cover_channels == 0){
      printf("Problem opening file, exiting\n");
      exit(-1);
    }

    // Image to be hidden
    unsigned int hidden_width, hidden_height, hidden_channels;
    unsigned char *hidden_data = stbi_load("./test_images/sample3.jpg", &hidden_width, &hidden_height, &hidden_channels, 0);
    if ( hidden_data == NULL || hidden_width*hidden_height*hidden_channels == 0){
      printf("Problem opening file, exiting\n");
      exit(-1);
    }

    // Check if image to be hidden fits in the cover image
    if ( hidden_width > cover_width || hidden_height > cover_height || hidden_width*hidden_height*hidden_channels > cover_width*cover_height*cover_channels){
      printf("The cover image is too small to fit the message\n");
      exit(-1);
    }

    // Change the cover image data to include the hidden image
    // The proper indexing is row*total_rows*total_channels + col*total_channels + channel
    for (unsigned int row = 0; row < cover_height; row++){
      for (unsigned int col = 0; col < cover_width; col++){
        for (unsigned int ch = 0; ch < cover_channels; ch++){
          unsigned int cover_index = row*cover_width*cover_channels+col*cover_channels+ch;
          if ( row < hidden_height && col < hidden_width){
            unsigned int hidden_index = row*hidden_width*hidden_channels+col*hidden_channels+ch;
            cover_data[cover_index] = ( cover_data[cover_index] & ~15 ) | ( hidden_data[hidden_index] & 15);
          } else {
            cover_data[cover_index] = ( cover_data[cover_index] & ~15 ) | ( 255 & 15);
          }
        }
      }
    }

    // Write the cover file with the hidden image
    stbi_write_jpg("out.jpg", cover_width, cover_height, cover_channels, cover_data, DESIRED_OUT_QUALITY_PERC);
    // Recover memory
    stbi_image_free(cover_data);
  }

  if (REVEAL_MODE){

    // Cover image information
    unsigned int cover_width, cover_height, cover_channels;
    unsigned char *cover_data = stbi_load("./out.jpg", &cover_width, &cover_height, &cover_channels, 0);
    if ( cover_data == NULL || cover_width*cover_height*cover_channels == 0){
      printf("Problem opening file, exiting\n");
      exit(-1);
    }

    for (unsigned int row = 0; row < cover_height; row++){
      for (unsigned int col = 0; col < cover_width; col++){
        for (unsigned int ch = 0; ch < cover_channels; ch++){
          unsigned int index = row*cover_width*cover_channels+col*cover_channels+ch;
          if ( cover_data[index] & 15 == 0){
            cover_data[index] = 255;
          } else {
            cover_data[index] <<= 4;
          }
        }
      }
    }
    stbi_write_jpg("hidden.jpg", cover_width, cover_height, cover_channels, cover_data, DESIRED_OUT_QUALITY_PERC);
    // Recover memory
    stbi_image_free(cover_data);
  }


  return 0;
}
