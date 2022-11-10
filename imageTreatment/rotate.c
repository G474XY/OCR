#include <stdio.h>
#include <math.h>
#include "hough.h"


void printMat(int m[], size_t w,size_t  h)
{
	size_t i =0;
	size_t j = 0;
	for (;i<h; i++)
	{
		j=0;
		for (;j<w; j++)
		{
			printf ("%i ", m[i*w +j]);
		}
		printf("\n");
	}
}


void Rotate (int m[], size_t w, size_t h, double angle)
{
	int m2[w*h];
	size_t i =0;
	size_t j = 0;
	double pi = 3.14159265;
	double val = pi/180.0;
	int decalx = 0;
	int decaly = 0;
	if (angle == 90)
	{
		decalx = w-1;
	}
	if (angle == 180)
	{
		decaly = h;
	}
	
	double c = cos( val * angle);
	double s = sin (val * angle);

//	double c = 0;
//	double s = 1;
	for (;i<h; i++)
	{
		j=0;
		for (;j<w; j++)
		{
			m2[i*w + j] = m[(int)((c*i - s*j)*w + (s*i+c*j))+decalx*w+decaly ];
			//printf("m2[%lu, %lu] : %i \n", i, j, m2[i*w+j]);
		}
	}


	i=0;
	for (; i<h*w; i++)
	{
		m[i] = m2[i];
	}

}

SDL_Surface* load_image(const char* path)
{
    SDL_Surface* s1 = IMG_Load(path);
    if (s1 == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface* s2 = SDL_ConvertSurfaceFormat(s1, SDL_PIXELFORMAT_RGB888, 0);
    if (s2 == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_FreeSurface(s1);
    return s2;
}


int main (int argc, char** argv)
{
	/*
	int m[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

	printMat(m, 4, 4);
	


	Rotate(m, 4, 4, 45);

	printf("-------------------------\n");
	printMat(m, 4,4);
*/

	// Checks the number of arguments.
     if (argc != 2)
         errx(EXIT_FAILURE, "Usage: image-file");

     //Initialize the SDL
     if (SDL_Init(SDL_INIT_VIDEO) != 0)
         errx(EXIT_FAILURE, "%s", SDL_GetError());

     // Creates a window.
     SDL_Window* window = SDL_CreateWindow("Dynamic Fractal Canopy", 0, 0,
       640, 400,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (window == NULL)
         errx(EXIT_FAILURE, "%s", SDL_GetError());

     // Creates a renderer.
     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
     SDL_RENDERER_ACCELERATED);
     if (renderer == NULL)
         errx(EXIT_FAILURE, "%s", SDL_GetError());

     // - Create a surface from the colored image.
     SDL_Surface* s = load_image(argv[1]);

     // - Resize the window according to the size of the image.
     SDL_SetWindowSize(window, s->w, s->h);

     size_t i = 0;
     size_t j =0;
     int* m  = hough(s, &i, &j);




     printMat(m, i, j);



     printf("%lu", i);


	return 0;

}
