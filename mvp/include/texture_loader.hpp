#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

unsigned char *load_texture_image(char *image, int *width, int *height, int *channels)
{
    return stbi_load(image, width, height, channels, 0);
}