#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdlib.h>


int* hough(SDL_Surface* surface, size_t* i1, size_t* j1)
{


	double pi = 3.14159265;
	double val = pi/180.0;

	SDL_LockSurface(surface);
	size_t len = surface->w*surface->h;

	double d = sqrt(surface->h*surface->h + surface->w*surface->w);
	double theta = 180.0;

	size_t len2 = (size_t)(2*d*theta);

	int* accumulator = malloc(sizeof(int)*len2);

	for (size_t i =0; i<len2; i++)
	{
		accumulator[i] = 0;

	}

	for (size_t i =0; i<surface->w;i++)
	{
		
		for (size_t j = 0; i<surface->h; j++)
		{
			
			Uint32* pixels = surface->pixels;
			Uint8 r,g,b;
			SDL_PixelFormat* format = surface->format;
			SDL_GetRGB(pixels[i*surface->w+j],format, &r,&g,&b);
			if (r == 255)
			{
		        	printf("%lu\n", j);
				for (int k =0; k<theta; k++)
				{
					double rho = i * cos(k*val) + j*sin(k*val);
					accumulator[(int)((rho)*2*d+k)]++;
				}
			}
			
		}
	}


	i1 = (size_t) d;
	j1 = (size_t)theta;

	return accumulator;

}
