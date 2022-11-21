//
// Created by ppaza on 24/06/2022.
//

#ifndef OCR_NN_H
#define OCR_NN_H

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

#endif //OCR_NN_H
