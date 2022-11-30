# ifndef _PRETRAITEMENT_H
# define _PRETRAITEMENT_H

#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>


struct coordonate
{
	int x1;
	int x2;
	int y1;
	int y2;
};


double* hough(SDL_Surface* surface, int* k1);


struct coordonate *coor(double *lignes, int *size);
# endif
