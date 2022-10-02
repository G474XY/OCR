//
// Created by ppaza on 03/09/2022.
// NN that learn xor from https://www.youtube.com/watch?v=LA4I3cWkp1E
//Note : utiliser les matrices pour machine learnia
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>

double sigmoid(double x) { return 1 / (1 + exp(-x)); }

double dSigmoid(double x) { return x * (1 - x); }

double init_weights() { return ((double) rand() / (double) RAND_MAX); }

void shuffle(int *array, size_t n) {
    if (n > 1) {
        for (size_t i = 0; i < n - 1; ++i) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

#define numInputs 2
#define numHiddenNodes 2
#define numOutputNodes 1
#define numTrainingSets 4

int main(void) // Remplacer LayerBias par vecteurs !
{
    const double learning_rate = 0.1f;

    double hiddenLayer[numHiddenNodes];
    double outputLayer[numOutputNodes];

    double hiddenLayerBias[numHiddenNodes];
    double outputLayerBias[numOutputNodes];

    double hiddenWeight[numInputs][numHiddenNodes];
    double outputWeight[numHiddenNodes][numOutputNodes];

    double training_inputs[numTrainingSets][numInputs] = {{0.0f, 0.0f},
                                                          {1.0f, 0.0f},
                                                          {0.0f, 1.0f},
                                                          {1.0f, 1.0f}};

    double training_outputs[numTrainingSets][numOutputNodes] = {{0.0f},
                                                                {1.0f},
                                                                {1.0f},
                                                                {0.0f}};

    for (int i = 0; i < numInputs; ++i) {
        for (int j = 0; j < numHiddenNodes; ++j) {
            hiddenWeight[i][j] = init_weights();
        }
    }

    for (int i = 0; i < numHiddenNodes; ++i) {
        for (int j = 0; j < numOutputNodes; ++j) {
            outputWeight[i][j] = init_weights();
        }
    }

    for (int i = 0; i < numOutputNodes; ++i) {
        outputLayerBias[i] = init_weights();
    }

    int trainingSetOrder[] = {0, 1, 2, 3};

    int numberOfEpochs = 1000;

    // Train the neural network for a number of epoch
    for (int epoch = 0; epoch < numberOfEpochs; ++epoch) {

        shuffle(trainingSetOrder, numTrainingSets);

        for (int x = 0; x < numTrainingSets; ++x) {
            int i = trainingSetOrder[x];

            //Forward pass

            //Compute hidden layer activation
            for (int j = 0; j < numHiddenNodes; ++j) {
                double activation = hiddenLayerBias[j];
                for (int k = 0; k < numInputs; ++k) {
                    activation += training_inputs[i][k] * hiddenWeight[k][j];
                }
                hiddenLayer[j] = sigmoid(activation);
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
    fputs("Final Hidden Weights \n", stdout);
    for (int j = 0; j < numHiddenNodes; ++j) {
        fputs("[  ", stdout);
        for (int k = 0; k < numInputs; ++k) {
            printf("%f", hiddenWeight[k][j]);
        }
        fputs("] ", stdout);
    }


    fputs("]\nFinal Hidden Biases\n", stdout);
    for (int j = 0; j < numHiddenNodes; ++j) {
        printf("%f", hiddenLayerBias[j]);
    }

    fputs("Final Output Weights \n", stdout);
    for (int j = 0; j < numOutputNodes; ++j) {
        fputs("[  ", stdout);
        for (int k = 0; k < numInputs; ++k) {
            printf("%f", outputWeight[k][j]);
        }
        fputs("] ", stdout);
    }

    fputs("]\nFinal Output Biases\n", stdout);
    for (int j = 0; j < numOutputNodes; ++j) {
        printf("%f", outputLayerBias[j]);
    }

    fputs("]  \n", stdout);

    return 0;
}