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

double* hough(SDL_Surface* surface, int* k1, int *Nrho2, int* Nthetha2, int **m2)
{
	int h=surface->h;
	int w = surface->w;
	double rho =1.0;
	double theta = 1.0;
	int Nthetha = (int)(180.0/theta);
	int Nrho = (int)((sqrt(h*h+w*w))/rho);
	double pi = 3.14159265;
	*Nthetha2 = Nthetha;
	*Nrho2 = Nrho;
	double dtheta = pi/Nthetha;
	double drho = (int)((sqrt(h*h+w*w))/Nrho);
	*m2 = calloc(sizeof(int),Nthetha*Nrho);
	int *m = *m2;


	for (int j =0; j<h; j++)
	{

		for(int i =0; i<w; i++)
		{

			Uint32* pixels = surface->pixels;
			Uint8 r,g,b;
			SDL_PixelFormat* format = surface->format;
			SDL_GetRGB(pixels[i*surface->w+j],format, &r,&g,&b);
			if (r >150)
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
			if (m[i*Nthetha+j] >80)
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

	//printf("%d\n", m[Nrho]);

	//free (m);

	lignes = realloc(lignes, k*sizeof(double));
	*k1=k;
	return lignes;

}




	
struct coordonate *coor(double *lignes, int *size, SDL_Surface *surface)
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

		if(verif_line(m, k, surface))
		{
			is_already(&k, m);

			k++;
		}
		//printf("%d/%d/%d/%d\n", m[k].x1, m[k].y1, m[k].x2, m[k].y2);
		//printf("%f/%f\n",theta, rho );

		


		if (k==ma)
		 {
		 	ma*=2;
		 	m = realloc(m, ma*sizeof(struct coordonate));
		 }

	}


	//printf("%d\n", surface->w/average(0, m, k));
	//printf("%d\n", surface->w);

	*size = k;
	return m;
}

void pile_append(int** pile, int* pixel, int* pile_size) 
{
  pile[*pile_size] = pixel;
  (*pile_size)++;
}


void coloriage_pixel_pile(int* image, int* result, int* shape, int seuil, int valeur, int** pile, int i, int j, int *pile_size) 
{
  image[j*shape[0]+i] = valeur;
  result[j*shape[0]+i] = 255;
  int voisins[4][2] = {{i+1,j},{i-1,j},{i,j-1},{i,j+1}};
  for (int k = 0; k < 4; k++) 
  {
    int* pixel = voisins[k];
    if (pixel[0] >= 0 && pixel[0] < shape[1] && pixel[1] >= 0 && pixel[1] < shape[0]) 
    {
      if (image[pixel[1]*shape[0]+pixel[0]] > seuil) 
      {
        image[pixel[1]*shape[0]+pixel[0]] = valeur;
        pile_append(pile, pixel, pile_size);
      }
    }
  }
}



double *analyse(int* image, int seuil, int *size, int* result, int Nx, int Ny)
{
  int shape[] = {Nx, Ny};
  result = calloc(sizeof(int), Nx*Ny);
  int compteur = -1;
  int capacity_pos = 32;
  double *positions = malloc((2 * sizeof(double))*capacity_pos);

  for (int y = 0; y < Ny; y++) 
  {
    for (int x = 0; x < Nx; x++) 
    {

      if (image[y*Ny+x] > seuil) 
      {
      	   //printf("%d\n", x);

        //printf("%f\n", image[y*Ny+x]);

        compteur++;

        int** pile = malloc(sizeof(int*) * Nx * Ny);
        int si = 0;
        int sj = 0;
        int npix = 0;
        int pixel[2] = {x, y};
        pile[0] = pixel;
        int pile_size = 1;
        while (pile_size > 0) 
        {
          int* pixel = pile[pile_size - 1];
          int i = pixel[0];
          int j = pixel[1];
          pile_size--;
          si += i;
          sj += j;
          npix++;
          coloriage_pixel_pile(image, result, shape, seuil, 0, pile, i, j, &pile_size);
        }
        if (capacity_pos == compteur)
        {
        	capacity_pos *=2;
        	positions = realloc(positions, (2 * sizeof(double))*capacity_pos);
        }
        double xb = si * 1.0 / npix;
        double yb = sj * 1.0 / npix;
        positions[compteur] = xb;
        positions[compteur+1] = yb;
        //printf("%d\n", npix);
        //positions[compteur][2] = (double)npix;

        free(pile);

      }
    }
  }

  positions = realloc(positions, (2 * sizeof(double))*compteur);
  *size = compteur;
  //printf("%f\n", positions[200]);
  return positions;
}


struct coordonate *coor2(double *lignes, int *size)
{

	int ma = 8;
	struct coordonate *m = malloc(ma*sizeof(struct coordonate));
	int k =0;

	for (int i =0; i<*size; i+=2)
	{
		//printf("%d\n", i);
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
	m = realloc(m, k*sizeof(struct coordonate));
	*size = k;
	return m;
}

int in_grid(int w, int h, int i, int j)
{
	return i>=0 && i<h && j>=0 && j<w;
}

int verif_line (struct coordonate *m, int k, SDL_Surface *surface)
{
	int h = surface->h;
	int w = surface->w;
	int c = 0;
	if ((m[k].x1-m[k].x2<25 && m[k].x1-m[k].x2>-25))
	{
		m[k].lignes = 0;

		//printf("li\n");
		int j = (m[k].x1 + m[k].x2)/2;
		c = h/5;


		int i =0;
		int bo= 0;


		while (c>0 && i<h)
		{

			bo = 0;
			int ibis = -25;
			while (ibis<=25 && bo!=1)
			{
				if (in_grid(w, h, j, (i+ibis)))
				{
					Uint32* pixels = surface->pixels;
					Uint8 r,g,b;
					SDL_PixelFormat* format = surface->format;
					SDL_GetRGB(pixels[(i+ibis)*w+j],format, &r,&g,&b);
					//printf("%d\n", r);
					if (r>150)
					{
						bo=1;

					}
										//b=1;
				}
				ibis++;


			}
/*
			if (bo==1)
				printf("%d\n", bo);*/
			if (bo !=1)
			{
				c--;	
			}

			i++;
		}

			//c=1;
	}
	else if ((m[k].y1-m[k].y2<25 && m[k].y1-m[k].y2>-25))
	{
		m[k].lignes = 1;

		//printf("li\n");
		int i = (m[k].y1 + m[k].y2)/2;
		c = h/5;


		int j =0;
		int bo= 0;


		while (c>0 && j<w)
		{

			bo = 0;
			int jbis = -25;
			while (jbis<=25 && bo!=1)
			{
				if (in_grid(w, h, j+jbis, i))
				{
					Uint32* pixels = surface->pixels;
					Uint8 r,g,b;
					SDL_PixelFormat* format = surface->format;
					SDL_GetRGB(pixels[i*w+j+jbis],format, &r,&g,&b);
					//printf("%d\n", r);
					if (r>150)
					{
						bo=1;

					}
										//b=1;
				}
				jbis++;
				if (bo !=1)
			{
				c--;	
			}

			j++;
		}


			}
	//printf("verif_line\n");


	}
	if (c>0)
		return 1;
	return 0;
}

void is_already(int *k, struct coordonate *m)
{
	int k1 = *k;
	for (int i =0; i<k1; i++)
	{
		if (m[i].lignes == m[k1].lignes)
		{
			if (m[i].lignes == 0 && m[i].x1-m[k1].x1>-20 && m[i].x1-m[k1].x1<20)
			{
				m[i].x1 = (m[i].x1+m[k1].x1)/2;
				m[i].x2 = (m[i].x2+m[k1].x2)/2;

				(*k)--;
				return;
			}
			if (m[i].lignes == 1 && m[i].y1-m[k1].y1>-20 && m[i].y1-m[k1].y1<20)
			{
				m[i].y1 = (m[i].y1+m[k1].y1)/2;
				m[i].y2 = (m[i].y2+m[k1].y2)/2;

				(*k)--;
				return;
			}
		}
	}

	
}

int average(int li, struct coordonate *m, int size)
{
	int k =0;
	int p =0;

	for (int i =0; i<size; i++)
	{
		if (m[i].lignes == li)
		{
			k++;

			if (li == 0)
			{
				p+=m[i].x1;
				//p+=m[i].x2;

			}
			else
			{
				p+=m[i].y1;
				//p+=m[i].y2;
			}
			
		}
	}


	return p/k;
}