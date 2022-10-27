#include "stdio.h"
#include "stdlib.h"
#include "NN.h"

/*
OBSOLETE
NE SERT A RIEN
PAS BESOIN DE COMPILER
*/

void print_array(double arr[],int size)
{
    printf("[");
    for(int i = 0; i < size; i++)
    {
        printf("%f",*(arr + i));
        if(i < size - 1)
            printf(", ");
    }
    printf("]\n");
}

void print_array_array(double *arr[],int size1,int size2)
{
    for(int i = 0; i < size1; i++)
    {
        print_array(arr[i],size2);
    }
}

//==============

double doublefromfile(FILE *file)
{
    double res = 0;
    fscanf(file,"%lf",&res);
    return res;
}

char doubleAfromfile(FILE *file,int size,double array[])
{
    /*
    Fills an array of size 'size' with doubles gotten from the file 'file'.
    Returns 1 if an error occured, 0 else
    */
    int i = 0;
    while(!feof(file) && i < size)
    {
        array[i] = doublefromfile(file);
        i++;
    }
    return i < size;
}

char doubleAAfromfile(FILE *file,int size1,int size2,double *array[])
{
    int i = 0;
    char err = feof(file);
    while(!err && i < size1)
    {
        err = doubleAfromfile(file,size2,array[i]) && feof(file);
        i++;
    }
    return err || i < size1;
}

//=============================Saving==========================

void doubletofile(double d, FILE *file)
{
    fprintf(file,"%f",d);
}

void doublearraytofile(double array[],int size,FILE *file)
{
    fprintf(file,"%s","{\n");
    for(int i = 0; i < size; i++)
    {
        fprintf(file,"%c",' ');
        doubletofile(array[i],file);
        fprintf(file,"%c",'\n');
    }
    fprintf(file,"%c",'}');
}

void doublearrayarraytofile(double *array[],int size1,int size2,FILE *file)
{
    fprintf(file,"%s","{\n");
    for(int i = 0; i < size1; i++)
    {
        doublearraytofile(array[i],size2,file);
        fprintf(file,"%c",'\n');
    }
    fprintf(file,"%c",'}');
}

//==============

int main()
{
    return 0;
}