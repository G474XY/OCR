#include <stdio.h>
#include <stdlib.h>
#include "NN.h"

//===============CONSTANTS================
const char* save_path = "save/Network.txt";
//========================================

void print_array(double* arr,size_t size)
{
    printf("[ ");
    for(size_t i = 0; i < size; i++)
        printf("%lf ",arr[i]);
    printf("]\n");
}

void print_neuron(neuron* neu)
{
    printf("{\nValue = %lf\nBias = %lf\nWeights = ",neu->value,neu->bias);
    print_array(neu->weight,neu->weight_length);
    printf("}\n");
}

void print_layer(layer* l)
{
    printf("[\nLength = %ld\n",l->length);
    for(long i = 0; i < l->length; i++)
        print_neuron((l->array[i]));
    printf("]\n");
}

void print_network(network* n)
{
    printf("Number of layers = %ld\n",n->length);
    for(long i = 0; i < n->length; i++)
        print_layer(n->array[i]);
}

void free_neuron(neuron* n)
{
    free(n->weight);
    free(n);
}

void free_layer(layer* l)
{
    for(long i = 0; i < l->length; i++)
        free_neuron((l->array[i]));
    free(l->array);
    free(l);
}

void free_network(network* n)
{
    for(long i = 0; i < n->length; i++)
        free_layer(n->array[i]);
    free(n->array);
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

neuron* load_neuron(FILE* file)
{
    if(read_char(file,'{'))
        return NULL;
    neuron* neu = malloc(sizeof(neuron));
    neu->value = doublefromfile(file);
    neu->bias = doublefromfile(file);
    neu->weight_length = doublefromfile(file);
    neu->weight = malloc(neu->weight_length * sizeof(double));
    if(doubleAfromfile(file,neu->weight_length,neu->weight)
    || read_char(file,'}'))
    {
        printf("Error while reading neuron weights\n");
        free_neuron(neu);
        return NULL;
    }
    
    return neu;
}

layer* load_layer(FILE* file)
{
    if(read_char(file,'['))
        return NULL;
    layer* lay = malloc(sizeof(layer));
    lay->length = doublefromfile(file);
    lay->array = malloc(lay->length * sizeof(neuron*));
    for(long i = 0; i < lay->length; i++)
    {
        neuron* n = load_neuron(file);
        if(n == NULL)
        {
            printf("Error while loading neuron of layer\n");
            free_layer(lay);
            return NULL;
        }
        lay->array[i] = n;
    }
    if(read_char(file,']'))
    {
        printf("Error while ending layer\n");
        free_layer(lay);
        return NULL;
    }

    return lay;
}

network* load_network_error(network* net)
{
    free_network(net);
    return NULL;
}

network* load_network(FILE* file)
{
    if(read_char(file,'('))
        return NULL;
    network* net = malloc(sizeof(network));
    net->length = doublefromfile(file);
    net->array = malloc(net->length * sizeof(layer*));
    for(long i = 0; i < net->length; i++)
    {
        layer* l = load_layer(file);
        if(l == NULL)
            return load_network_error(net);
        net->array[i] = l;
    }
    if(read_char(file,')'))
        return load_network_error(net);
    return net;
}

//========================================

//=================SAVING=================

void doubletofile(double d, FILE *file)
{
    fprintf(file,"%f",d);
}

void doubleAtofile(double *array,int size,FILE *file)
{
    fprintf(file,"%s","[ ");
    for(int i = 0; i < size; i++)
    {
        doubletofile(array[i],file);
        fprintf(file,"%c",' ');
    }
    fprintf(file,"%s","]\n");
}

void save_neuron(neuron* neu,FILE* file)
{
    fprintf(file,"{\n");
    doubletofile(neu->value,file);
    fprintf(file,"\n");
    doubletofile(neu->bias,file);
    fprintf(file,"\n");
    doubletofile(neu->weight_length,file);
    fprintf(file,"\n");
    doubleAtofile(neu->weight,neu->weight_length,file);
    fprintf(file,"}\n");
}

void save_layer(layer* lay,FILE* file)
{
    fprintf(file,"[\n");
    doubletofile(lay->length,file);
    fprintf(file,"\n");
    for(long i = 0; i < lay->length; i++)
        save_neuron(lay->array[i],file);
    fprintf(file,"]\n");
}

void save_network(network* net,FILE* file)
{
    fprintf(file,"(\n");
    doubletofile(net->length,file);
    fprintf(file,"\n");
    for(long i = 0; i < net->length; i++)
        save_layer(net->array[i],file);
    fprintf(file,")\n");
}

//========================================

//===============FUNCTIONS================

network* load_network()
{
    FILE* file = fopen(save_path,"r");
    network* net = load_network(file);
    fclose(file);
    return net;
}

void save_network(network* net)
{
    FILE *file = fopen(save_path,"w");
    save_network(net,file);
    fclose(file);
}