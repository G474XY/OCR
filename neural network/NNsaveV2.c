#include <stdio.h>
#include "NNsaveV2.h"

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

char readnchars(FILE *file,int n)
{
    int i = 0;
    while(i < n && !feof(file))
    {
        i++;
    }
    return i < n;
}

char gchar(FILE *file)
{
    /*
    Pour éviter la suite de caractères 13+10 : carriage return line feed.
    13 -> \n et 10 -> line feed. des fois ce truc pop jsp pk, donc cette
    fonction y remédie.
    */
    char c = fgetc(file);
    //printf("(%d) : %c\n",c,c);
    //while(c == 13)
        //c = fgetc(file);
    return c;
}

double* init_arr(int size)
{
    double* res = malloc(size * sizeof(double));
    for(int i = 0; i < size; i++)
        res[i] = 0;
    return res;
}

double** init_2arr(int size1, int size2)
{
    double** res = malloc(size1 * sizeof(double));
    for(int i = 0; i < size1; i++)
    {
        res[i] = init_arr(size2);
    }
    return res;
}

double*** init_3arr(int size1, int size2, int size3)
{
    double*** res = malloc(size1 * sizeof(double));
    for(int i = 0; i < size1; i++)
    {
        res[i] = init_2arr(size2,size3);
    }
    return res;
}

void free_2arr(double **arr,int size1)
{
    for(int i = 0; i < size1; i++)
        free(arr[i]);
}

void free_3arr(double ***arr,int size1, int size2)
{
    for(int i = 0; i < size1; i++)
        free_2arr(arr[i],size2);
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
    gchar(file); // {
    gchar(file); // \n
    int i = 0;
    while(!feof(file) && i < size)
    {
        array[i] = doublefromfile(file);
        gchar(file); //Le \n
        i++;
    }
    gchar(file); // }
    gchar(file); // \n
    return i < size;
}

char doubleAAfromfile(FILE *file,int size1,int size2,double **array)
{
    //readnchars(file,2); // '[\n'
    gchar(file); // '['
    gchar(file); // '\n'
    int i = 0;
    char err = feof(file);
    while(!err && i < size1)
    {
        err = doubleAfromfile(file,size2,array[i]);
        i++;
    }
    //err = readnchars(file,2) || err; // ']\n'
    gchar(file); //']'
    gchar(file); //'\n'
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
        //fprintf(file,"%c",' ');
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


void test()
{
    int size = 6;
    double *arr = init_arr(size);

    FILE *file = fopen("savetests/double.txt","r");
    doubleAfromfile(file,size,arr);
    print_array(arr,size);

    free(arr);
}

void test2()
{
    int size1 = 3;
    int size2 = 3;
    double arr1[] = {3,2,81};
    double arr2[] = {8,9,7};
    double arr3[] = {65,42,21};
    double arr4[] = {77,11,22};
    double arr5[] = {4,900,61.2};
    double arr6[] = {3.14,5.5,42.42};

    double **arr7 = malloc(size1*sizeof(double));
    arr7[0] = arr1;
    arr7[1] = arr2;
    arr7[2] = arr3;
    
    double **arr8 = malloc(size1*sizeof(double));
    arr8[0] = arr4;
    arr8[1] = arr5;
    arr8[2] = arr6;

    FILE *file = fopen("savetests/test.txt","w");
    doubleAAtofile(arr7,size1,size2,file);
    doubleAAtofile(arr8,size1,size2,file);
    fclose(file);
    free(arr7);
    free(arr8);

    double **arr9 = malloc(size1 * sizeof(double));
    arr9[0] = malloc(size2 * sizeof(double));
    arr9[1] = malloc(size2 * sizeof(double));
    arr9[2] = malloc(size2 * sizeof(double));
    double **arr10 = init_2arr(size1,size2);

    FILE *file2 = fopen("savetests/test.txt","r");
    doubleAAfromfile(file2,size1,size2,arr9);
    doubleAAfromfile(file2,size1,size2,arr10);
    fclose(file2);
    print_array_array(arr9,size1,size2);
    printf("\n");
    print_array_array(arr10,size1,size2);

    free(arr9[0]);
    free(arr9[1]);
    free(arr9[2]);
    free(arr9);
    free_2arr(arr10,size1);
}

//==============

/*int main()
{
    test2();
    
    return 0;
}*/