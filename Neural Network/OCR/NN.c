//
// Created by ppaza on 24/06/2022.
//

#include "NN.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define numInputs 2
#define numLayer 1
#define numHiddenNodes 4
#define numOutputs 1
#define numTrainingSets 4

double hiddenLayer[numLayer][numHiddenNodes];
double outputLayer[numLayer][numOutputs];

double hiddenLayerBias[numLayer][numHiddenNodes];
double outputLayerBias[numLayer][numOutputs];

double hiddenWeight[numLayer][numHiddenNodes][numInputs];
double outputWeight[numLayer][numHiddenNodes][numOutputs];

double sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

double dSigmoid(double x) {
    return x * (1 - x);
}

double init_weights() {
    return ((double) rand() / (double) RAND_MAX);
}

void initialisation(double *weight)
{
    for(size_t i = 0; i < numLayer; ++i){
        for (int j = 0; j < numHiddenNodes; ++j) {
            for (int k = 0; k < numInputs; ++k) {
                hiddenWeight[i][j][k] = init_weights();
            }
        }
    }
}