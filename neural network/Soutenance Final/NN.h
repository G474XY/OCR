#ifndef OCR_NN_H
#define OCR_NN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

typedef struct neuron neuron;
struct neuron{
    long weight_length;
    double* weight;
    double bias;
    double value;
};

typedef struct layer layer;
struct layer{
    neuron** array;
    long length;
};

typedef struct network network;
struct network {
    layer** array;
    long length;
};

int neural_network(double* input, char train);

#endif