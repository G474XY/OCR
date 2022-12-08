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
	int lignes;
};

struct pixel
{
	int x;
	int y;
};

struct pixels
{
	struct pixel *pix;
	int size;
	int capacity;
};

struct pos
{
	double x;
	double y;
	int n;
};

struct poss
{
	struct pos *poss;
	int size;
	int capacity;
};


double* hough(SDL_Surface* surface, int* k1, int *Nrho2, int* Nthetha2, int **m2);


void color_pixel(int *im, int *res, struct pixels *pile, int *i1, int *j1, int seuil, int val, int i, int j);

struct coordonate *coor(double *lignes, int *size, SDL_Surface *surface);

void pile_append(int** pile, int* pixel, int* pile_size);

void coloriage_pixel_pile(int* image, int* result, int* shape, int seuil, int valeur, int** pile, int i, int j, int *pile_size);

double *analyse(int* image, int seuil, int *size, int* result, int Nx, int Ny);

struct coordonate *coor2(double *lignes, int *size);

int in_grid(int w, int h, int i, int j);

int verif_line (struct coordonate *m, int k, SDL_Surface *surface);

void insert_coor(int i, struct coordonate **m, int size, struct coordonate *cor);
void is_already(int *k, struct coordonate *m);
void remove_coor(int i, struct coordonate *m);
void average(int li, struct coordonate *m, int size, int w, int h);


# endif
