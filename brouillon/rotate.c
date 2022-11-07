#include <stdio.h>
#include <math.h>


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
			m2[i*w + j] = m[(int)((c*i - s*j)*w + (s*i+c*j))+decalx*w ];
			//printf("m2[%lu, %lu] : %i \n", i, j, m2[i*w+j]);
		}
	}


	i=0;
	for (; i<h*w; i++)
	{
		m[i] = m2[i];
	}

}


int main ()
{
	int m[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

	//printMat(m, 4, 4);

	Rotate(m, 4, 4, 90.0);

	printMat(m, 4,4);


	return 0;

}
