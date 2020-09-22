#include "../headers/steg.h"

int main(int argc, char *argv[])
{

  struct config_struct configurations;
  init_configs(&configurations);

  process_arguments(argc, argv, &configurations);

  if (configurations.encrypt_flag)
    hide_png_in_png(configurations);

  if (configurations.decrypt_flag)
    extract_png_from_png(configurations);

  return 0;
}

//TODO just keep the RGBA if both are RGBA
//TODO combine alpha instead of removing it in RGBA->RGB
//TODO update README
