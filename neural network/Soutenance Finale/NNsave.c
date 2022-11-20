#include <stdio.h>
#include <stdlib.h>
#include "NN.h"

void print_array(double* arr,size_t size)
{
    printf("[ ");
    for(size_t i = 0; i < size; i++)
        printf("%lf ",arr[i]);
    printf("]\n");
}

void print_neuron(neuron* neu,size_t w_size)
{
    printf("{\nValue = %lf\nBias = %lf\nWeights = ",neu->value,neu->bias);
    print_array(neu->weight,w_size);
    printf("}\n");
}

void free_neuron(neuron* n)
{
    free(n->weight);
    free(n);
}

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

//========================================

//================LOADING=================

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
    read_char(file,'[');
    int i = 0;
    while(!feof(file) && i < size)
    {
        array[i] = doublefromfile(file);
        fgetc(file); //Le ' '
        i++;
    }
    read_char(file,']');
    return i < size;
}

neuron* load_neuron(FILE* file,size_t w_len)
{
    if(read_char(file,'{'))
        return NULL;
    neuron* neu = malloc(sizeof(neuron));
    neu->value = doublefromfile(file);
    neu->bias = doublefromfile(file);
    neu->weight = malloc(w_len * sizeof(double));
    if(doubleAfromfile(file,w_len,neu->weight) || read_char(file,'}'))
    {
        printf("Error while reading neuron weights\n");
        free(neu->weight);
        free(neu);
        return NULL;
    }
    
    return neu;
}

//layer* load_layer(FILE* file)

int main()
{
    FILE* file = fopen("save_tests/neuron.txt","r");
    neuron* n = load_neuron(file,4);
    if(n == NULL)
        printf("Error while loading neuron\n");
    else
    {
        print_neuron(n,4);
        free_neuron(n);
    }
    return 0;
}