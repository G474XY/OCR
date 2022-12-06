# ifndef _PRETRAITEMENT_H
# define _PRETRAITEMENT_H

#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


SDL_Surface * copy_s(SDL_Surface* source);
Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format);
void surface_to_grayscale(SDL_Surface* surface);
Uint32 moyenne(SDL_Surface *surface, int i, int j, int n);

void flou_gaussien(SDL_Surface* surface);
void detourage(SDL_Surface* surface);

double find_threshold(SDL_Surface* surface, int h1, int h2, int w1, int w2);
void binarization(SDL_Surface* surface, int n);
void white_to_black(SDL_Surface* surface);

void change_luminosity(SDL_Surface* surface);
Uint8 luminosity(Uint8 c, double n);

void change_contrast(SDL_Surface* surface);
Uint8 contrast(Uint8 c, double n);

double convolution(SDL_Surface * s, double kernel[3][3], int row, int col);
void sobel(SDL_Surface* surface);

# endif
