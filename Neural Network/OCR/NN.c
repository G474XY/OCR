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

//Calcule l'erreur entre la sortie et ce qui été attendu
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

void update(double dw, double db, double *w[], double *b, int learning_rate){

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

void shuffle(int *array, size_t n){
    if (n > 1) {
        for (size_t i = 0; i < n - 1; ++i) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

//Forward propagation

// si Training
    //Back propagation

    //Update

//+-+-+-+-+-+-+-+-+Fonctions pour l'exploitation+-+-+-+-+-+-+-+-+

int main(){

    const double learning_rate = 0.1f;

    // Faire un malloc
    //Stock A = sigmoid(- X*W+B)
    double hiddenLayer[numHiddenLayer][numHiddenNodes];
    double outputLayer[numOutputNodes];

    //Stock B
    double hiddenLayerBias[numHiddenLayer][numHiddenNodes];
    double outputLayerBias[numOutputNodes];

    //Stock W
    double hiddenWeight[numHiddenLayer][numHiddenNodes][numInputs];
    double outputWeight[numHiddenNodes][numOutputNodes];

    
    double training_inputs[numTrainingSets][numInputs] = {{0.0f, 0.0f},
                                                          {1.0f, 0.0f},
                                                          {0.0f, 1.0f},
                                                          {1.0f, 1.0f}};

    double training_outputs[numTrainingSets][numOutputNodes] = {{0.0f},
                                                                {1.0f},
                                                                {1.0f},
                                                                {0.0f}};

    int trainingSetOrder[] = {0, 1, 2, 3};

    initialisation();

    int numberOfEpochs = 1000;

    // Train the neural network for a number of epoch
    for (int epoch = 0; epoch < numberOfEpochs; ++epoch) {

        shuffle(trainingSetOrder, numTrainingSets);

        for (int x = 0; x < numTrainingSets; ++x) {
            int i = trainingSetOrder[x];

            //Forward pass

            //Compute hidden layer activation
            for (int j = 0; j < numHiddenLayer; ++j) {
                for (int k = 0; k < numHiddenNodes; ++j) {
                    double activation = hiddenLayerBias[j][k];
                    for (int l = 0; l < numInputs; ++l) {
                        activation += training_inputs[i][l] * hiddenWeight[j][k][l];
                    }
                    hiddenLayer[j][k] = sigmoid(activation);
            }


            //Compute output layer activation
                for (int j = 0; j < numOutputNodes; ++j) {
                    double activation = hiddenLayerBias[j];
                    for (int k = 0; k < numHiddenNodes; ++k) {
                        activation += hiddenLayer[k] * outputWeight[k][j];
                    }
                    outputLayer[j] = sigmoid(activation);
                }

            // Print result from forward pass
            printf("Input: %g   Output : %g   Predicted Output: %g  \n",
                   training_inputs[i][0], training_inputs[i][1], outputLayer[0], training_outputs[i][0]);

            // Backpropagation

            // Compute change in ouput weights
            double deltaOutput[numOutputNodes];

            for (int j = 0; j < numOutputNodes; ++j) {
                double error = (training_outputs[i][j] - outputLayer[j]);
                deltaOutput[j] = error * dSigmoid(outputLayerBias[j]);
            }

            // Compute change in hidden weights
            double deltaHidden[numHiddenNodes];

            for (int j = 0; j < numHiddenNodes; ++j) {
                double error = 0.0f;
                for (int k = 0; k < numOutputNodes; ++k) {
                    error += deltaOutput[k] * outputWeight[j][k];
                }
                deltaHidden[j] = error * dSigmoid(hiddenLayer[j]);
            }

            // Apply change in output weights
            for (int j = 0; j < numOutputNodes; ++j) {
                outputLayerBias[j] += deltaOutput[j] * learning_rate;
                for (int k = 0; k < numHiddenNodes; ++k) {
                    outputWeight[k][j] += hiddenLayer[k] * deltaOutput[j] * learning_rate;
                }
            }

            // Apply change in hidden weights
            for (int j = 0; j < numHiddenNodes; ++j) {
                hiddenLayerBias[j] += deltaHidden[j] * learning_rate;
                for (int k = 0; k < numHiddenNodes; ++k) {
                    hiddenWeight[k][j] += training_inputs[i][k] * deltaHidden[j] * learning_rate;
                }
            }


        }

    }

    return 0;
}

