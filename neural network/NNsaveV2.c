#include <stdio.h>
#include "NNsaveV2.h"

/*
Detects the next occurence of 'to_read' in the file 'file'.
Returns 1 if the file ended before that, 0 else.
*/
char read_char(FILE *file,char to_read)
{
    char cont = 2;
    char c = 0;
    while(cont > 1)
    {
        c = fgetc(file);
        if(c == EOF)
            cont = 1;
        if(c == to_read)
            cont = 0;
    }
    if(cont)
        printf("read_char : reached EOF while looking for %c\n",to_read);
    return cont;
}

//==============

double doublefromfile(FILE *file)
{
    double res = 0;
    int fres = fscanf(file,"%lf",&res);
    fres++;
    return res;
}

char doubleAfromfile(FILE *file,int size,double *array)
{
    /*
    Fills an array of size 'size' with doubles gotten from the file 'file'.
    Returns 1 if an error occured, 0 else
    */
    read_char(file,'{');
    int i = 0;
    while(!feof(file) && i < size)
    {
        array[i] = doublefromfile(file);
        gchar(file); //Le ' '
        i++;
    }
    read_char(file,'}');
    return i < size;
}

char doubleAAfromfile(FILE *file,int size1,int size2,double **array)
{
    read_char(file,'[');
    int i = 0;
    char err = feof(file);
    while(!err && i < size1)
    {
        err = doubleAfromfile(file,size2,array[i]);
        i++;
    }
    read_char(file,']');
    return err || i < size1;
}

//=============================Saving==========================

void doubletofile(double d, FILE *file)
{
    fprintf(file,"%f",d);
}

void doubleAtofile(double *array,int size,FILE *file)
{
    fprintf(file,"%s","{ ");
    for(int i = 0; i < size; i++)
    {
        doubletofile(array[i],file);
        fprintf(file,"%c",' ');
    }
    fprintf(file,"%s","}\n");
}

void doubleAAtofile(double **array,int size1,int size2,FILE *file)
{
    fprintf(file,"%s","[\n");
    for(int i = 0; i < size1; i++)
    {
        doubleAtofile(array[i],size2,file);
        //fprintf(file,"%c",'\n');
    }
    fprintf(file,"%s","]\n");
}
