#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdlib.h>
#include "hough.h"

/*
int* hough(SDL_Surface* surface, size_t* i1, size_t* j1)
{


	double pi = 3.14159265;
	double val = pi/180.0;

	SDL_LockSurface(surface);
	//int len = surface->w*surface->h;

	double d = sqrt(surface->h*surface->h + surface->w*surface->w);
	double theta = 180.0;

	int len2 = (int)(2*d*theta);

	int* accumulator = malloc(sizeof(int)*len2);

	for (int i =0; i<len2; i++)
	{
		accumulator[i] = 0;

	}


	for (int i =0; i<surface->w;i++)
	{
		
		for (int j = 0; j<surface->h; j++)
		{
			
			Uint32* pixels = surface->pixels;
			Uint8 r,g,b;
			SDL_PixelFormat* format = surface->format;
			SDL_GetRGB(pixels[i*surface->w+j],format, &r,&g,&b);
			if (r == 0)
			{
				for (int k =0; k<theta; k++)
				{
					double rho = i * cos(k*val) + j*sin(k*val);
					if ((int)((rho)*2*d+k)<len2)
						accumulator[(int)((rho)*2*d+k)]++;
				}
			}
			
		}
	}





	*i1 = (size_t) d;
	*j1 = (size_t)theta;

	printf("%ln", i1);
	return accumulator;

}
*/

double* hough(SDL_Surface* surface, int* k1)
{
	int h=surface->h;
	int w = surface->w;
	double rho =1.0;
	double theta = 1.0;
	int Nthetha = (int)(180.0/theta);
	int Nrho = (int)((sqrt(h*h+w*w))/rho);
	double pi = 3.14159265;
	double dtheta = pi/Nthetha;
	double drho = (int)((sqrt(h*h+w*w))/Nrho);
	int *m = calloc(Nthetha*Nrho, sizeof(int));


	for (int j =0; j<h; j++)
	{

		for(int i =0; i<w; i++)
		{

			Uint32* pixels = surface->pixels;
			Uint8 r,g,b;
			SDL_PixelFormat* format = surface->format;
			SDL_GetRGB(pixels[i*surface->w+j],format, &r,&g,&b);
			if (r <100)
			{

				for (int i_theta =0; i_theta<Nthetha; i_theta++)
				{


					theta = i_theta*dtheta;
					rho = i*cos(theta)+(w-j)*sin(theta);
					double i_rho = (int)(rho/drho);
					//printf("%d\n", i_theta + (int)(i_rho)*Nrho);
					if ((i_rho>0) && (i_rho<Nrho))
						m[i_theta*Nthetha + (int)(i_rho)] ++;
				}
			}
		}
	}


	int ma =32;
	int k =0;
	double *lignes = malloc(ma*sizeof(double));

	for (int i =0; i<Nthetha; i++)
	{
		for (int j =0; j<Nrho; j++)
		{
			if (m[i*Nthetha+j] >1700)
			{
				lignes[k] = j*drho;
				lignes[k+1] = i*dtheta;
				k+=2;
				if(k==ma)
				{
					ma*=2;
					lignes = realloc(lignes, ma*sizeof(double));
				}
			}
		}
	}

	//printf("%d\n", k);


	free (m);

	*k1=k;
	return lignes;

}



struct coordonate *coor(double *lignes, int *size)
{

	int ma = 8;
	struct coordonate *m = malloc(ma*sizeof(struct coordonate));
	int k =0;

	for (int i =0; i<*size; i+=2)
	{
		double rho = lignes[i];
		double theta = lignes[i+1];

		double a = cos(theta);
		double b = sin(theta);
		double x0 = a*rho;
		double y0 = b*rho;

		//m[k] = malloc(sizeof(struct coordonate));
		m[k].x1 = (int)(x0+1000*(-b));
		m[k].y1 = (int)(y0+1000*(a));
		m[k].x2 = (int)(x0-1000*(-b));
		m[k].y2 = (int)(y0-1000*(a));
		//printf("%d/%d/%d/%d\n", m[k].x1, m[k].y1, m[k].x2, m[k].y2);
		//printf("%f/%f\n",theta, rho );
		k++;
		if (k==ma)
		 {
		 	ma*=2;
		 	m = realloc(m, ma*sizeof(struct coordonate));
		 }

	}

	

	*size = k;
	return m;
}