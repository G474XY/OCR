

# ifndef _PRETRAITEMENT_H
# define _PRETRAITEMENT_H

#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define grayscale_path "tmp/s1.png"
#define lighting_path "tmp/s2.png"
#define contrast_path "tmp/s3.png"
#define binarisation_path "tmp/s4.png"
#define invert_path "tmp/s5.png"
#define sobel_path "tmp/s6.png"
#define rotated_path "tmp/rotated.png"


void load_image(const char* path);
void rotate_image(const char* path, double angle);

# endif

