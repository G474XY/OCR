//
// Created by ppaza on 24/06/2022.
// Source: https://www.youtube.com/playlist?list=PLO_fdPEVlfKoanjvTJbIbd9V5d9Pzp8Rw
// Note : pas sûr de la valeur pour les boucles => à refaire
//

#include "NN.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define numInputs 2
#define numLayer 1
#define numHiddenNodes 2
#define numOutputs 1
#define numTrainingSets 4

//Stock A = sigmoid(- X*W+B)
double hiddenLayer[numLayer][numHiddenNodes];
double outputLayer[numLayer][numOutputs];

//Stock B
double hiddenLayerBias[numLayer][numHiddenNodes];
double outputLayerBias[numLayer][numOutputs];

//Stock W
double hiddenWeight[numLayer][numHiddenNodes][numInputs];
double outputWeight[numLayer][numHiddenNodes][numOutputs];


//+-+-+-+-+-+-+-+-+Fonctions pour neurones+-+-+-+-+-+-+-+-+
double sigmoid(double z) {
    return 1 / (1 + exp(-z));
}

//Calcule la sortie du neurone
double model(double x[], double w[], double b) {
    double z = 0;
    for (int i = 0; i < numInputs; ++i) {
        z += w[i] * x[i];
    }
    z += b;
    return sigmoid(z);
}

//Calcule l'erreur entre la sorite et ce qui été attendu
double log_loss(double a[], double y[]){
    double l = 0;

        for (int i = 0; i < numTrainingSets; ++i) {
            l += y[i] * log(a[i]) + (1 - y[i]) * log(1 - a[i]);
        }

        return -(1/numTrainingSets) * l;
}

//Calcule de la descente de gradient
void gradients(double a[], double x[], double y[], double *dw, double *db){
    *dw = 0;
    *db = 0;
    for (int i = 0; i < numTrainingSets; ++i) {
        *dw += x[i] * (a[i] - y[i]);
        *db += a[i] - y[i];
    }

    *db *= 1/numTrainingSets;
    *dw *= 1/numTrainingSets;
}

void update(double dw, double db, double *w[], double *b, double learning_rate){ //Non complete

    for (int i = 0; i < numTrainingSets; ++i)
        *w[i] = *w[i] - learning_rate * dw;

    *b = *b - learning_rate * db;
}

//+-+-+-+-+-+-+-+-+Fonctions pour réseau+-+-+-+-+-+-+-+-+


//Fonctions d'initialisation
double random() {
    return ((double) rand() / (double) RAND_MAX);
}

void initialisation(double *weight)
{

}

//+-+-+-+-+-+-+-+-+Fonctions pour l'exploitation+-+-+-+-+-+-+-+-+

void artificial_neuron(double x[], double y[]){

}