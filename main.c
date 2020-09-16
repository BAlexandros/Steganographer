#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "headers/stb_image.h"
#include "headers/stb_image_write.h"

#define DESIRED_OUT_QUALITY_PERC 100

int main(void)
{
  unsigned int width,height,channels;
  unsigned char *data = stbi_load("test_images/sample1.jpeg", &width, &height, &channels, 3);
  for (unsigned int i = 0; i < channels*height*width; i += channels){
    // Do something with the pixels
  }
  stbi_write_jpg("out.jpg", width, height, channels, data, DESIRED_OUT_QUALITY_PERC);

  stbi_image_free(data);

  return 0;
}
