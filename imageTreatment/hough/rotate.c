#include <err.h>
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void Rotateimg (SDL_Surface* surface, double angle)
{


	int h = surface->h;
	int w = surface->w;

	int* mask = calloc((h*w), sizeof(int));


	int i = 0;
	int j = 0;
	double pi = 3.14159265359;
	double val = pi/180.0;
	int decalx = 0;
	int decaly = 0;
	if (angle == 90)
	{
		decalx = w-1;
	}
	if (angle == 45)
	{
		decalx = w/2;
		decaly = h/4;
	}
	if (angle == 180)
	{
		decaly = h-1;
	}
	if (angle == -90)
	{
		decalx = 1;
	}
	if (angle == -180)
	{
		decaly = h;
	}
	if(angle == 1)
		decalx = 10;

	double c = cos( val * angle);
	double s = sin (val * angle);

	//      double c = 0;
	//      double s = 1;
	Uint32* pixels = surface->pixels;
	for (;i<h; i++)
	{
		j=0;
		for (;j<w; j++)
		{

			int v = ((int)((c*i - s*j)*w + (s*i+c*j))+decalx*w+decaly*h);

			Uint8 r,g,b;
			if (v<w*h && v>=0)
			{
				
				SDL_GetRGB(pixels[v], surface->format, &r, &g, &b);
				mask[i*w + j] = r;
			}
			else
			{
				printf("%d\n", v);

				r=255;

			}

			
			//printf("m2[%lu, %lu] : %i \n", i, j, m2[i*w+j]);
		}
	}


	i=0;
	SDL_LockSurface(surface);

	for (; i<h*w; i++)
	{
		Uint8 ma =(Uint8)mask[i];
		pixels[i] = SDL_MapRGB(surface->format, ma, ma, ma);
	}

	SDL_UnlockSurface(surface);

	free(mask);

}
