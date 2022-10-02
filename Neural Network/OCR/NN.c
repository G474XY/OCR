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
#define numHiddenLayer 1
#define numHiddenNodes 1

#define numOutputNodes 0

#define numTrainingSets 4

#define learning_rate 0.01
#define numEpoch

//Stock A = sigmoid(- X*W+B)
double hiddenLayer[numHiddenLayer][numHiddenNodes];
double outputLayer[numOutputNodes];

//Stock B
double hiddenLayerBias[numHiddenLayer][numHiddenNodes];
double outputLayerBias[numOutputNodes];

//Stock W
double hiddenWeight[numHiddenLayer][numHiddenNodes][numInputs];
double outputWeight[numHiddenNodes][numOutputNodes];


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

void update(double dw, double db, double *w[], double *b){

    for (int i = 0; i < numInputs; ++i)
        *w[i] = *w[i] - learning_rate * dw;

    *b = *b - learning_rate * db;
}

//+-+-+-+-+-+-+-+-+Fonctions pour réseau+-+-+-+-+-+-+-+-+


//Fonctions d'initialisation
double random() {
    return ((double) rand() / (double) RAND_MAX);
}

void initialisation()
{
    // initialisation hidden layer
    for (int i = 0; i < numHiddenLayer; ++i) {
        for (int j = 0; j < numHiddenNodes; ++j) {
            for (int k = 0; k < numInputs; ++k) {
                hiddenWeight[i][j][k] = random();
            }
            hiddenLayerBias[i][j] = random();
            hiddenLayer[i][j] = random();
        }
    }

    // initialisation output layer avec un seul layer de sortie
    for (int i = 0; i < numHiddenNodes; ++i) {
        for (int j = 0; j < numOutputNodes; ++j) {
            outputWeight[i][j] = random();
        }

        outputLayerBias[i] = random();
        outputLayer[i] = random();
    }
}

//Forward propagation

//Back propagation

//Update

//+-+-+-+-+-+-+-+-+Fonctions pour l'exploitation+-+-+-+-+-+-+-+-+

void artificial_neuron(double x[], double y[]){
    //Initialisation
    initialisation();

    for (int j = 0; j < numEpoch; ++j) {
        
    }

}