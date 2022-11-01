#include <stdio.h>

#ifndef NNSAVEV2
#define NNSAVEV2

char doubleAfromfile(FILE *file,int size,double *array);
char doubleAAfromfile(FILE *file,int size1,int size2,double **array);
void doubleAtofile(double *array,int size,FILE *file);
void doubleAAtofile(double **array,int size1,int size2,FILE *file);

//double* init_arr(int size);
//double** init_2arr(int size1, int size2);
//void free_2arr(double **arr,int size1);

#endif