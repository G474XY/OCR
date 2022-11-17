//
// Created by ppaza on 24/06/2022.
//

#ifndef OCR_NN_H
#define OCR_NN_H

typedef struct neuron neuron;
struct neuron{
    double* weight;
    double bias;
    double value;
};

typedef struct layer layer;
struct layer{
    neuron* array;
    long length;
};

#endif //OCR_NN_H
