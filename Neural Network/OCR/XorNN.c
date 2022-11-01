#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "NNsaveV2.h"

#define numInputs 2
#define numHiddenNodes 2
#define numOutputs 1
#define numOutputsNodes 1
#define numTrainingSets 4

//==========DEBUT DE LA PARTIE DE B*****N==========
//==========LOADING==========

double init_weight();

char loadNN(char *path,double *hiddenLayer, double *outputLayer,
            double *hiddenLayerBias, double *outputLayerBias,
            double **hiddenWeights, double **outputWeights)
{
    FILE *file = fopen(path,"r");
    if(file == NULL)
    {
        printf("Bad path to load neural network from. Aborting operation.\n");
        return 1;
    }
    char res = 0;
    res = res || doubleAfromfile(file,numHiddenNodes,hiddenLayer);
    res = res || doubleAfromfile(file,numOutputsNodes,outputLayer);
    res = res || doubleAfromfile(file,numHiddenNodes,hiddenLayerBias);
    res = res || doubleAfromfile(file,numOutputs,outputLayerBias);
    res = res || doubleAAfromfile(file,numInputs,numHiddenNodes,hiddenWeights);
    res = res || doubleAAfromfile(file,numHiddenNodes,numOutputs,outputWeights);
    fclose(file);
    if(res)
    {
        printf("Something went wrong during file analysis.\n");
        return 1;
    }

    return 0;
}

//==========SAVING==========

void saveNN(char *path,double *hiddenLayer, double *outputLayer,
            double *hiddenLayerBias, double *outputLayerBias,
            double **hiddenWeights, double **outputWeights)
{
    FILE *file = fopen(path,"w");

    doubleAtofile(hiddenLayer,numHiddenNodes,file);
    doubleAtofile(outputLayer,numOutputsNodes,file);
    doubleAtofile(hiddenLayerBias,numHiddenNodes,file);
    doubleAtofile(outputLayerBias,numOutputs,file);
    doubleAAtofile(hiddenWeights,numInputs,numHiddenNodes,file);
    doubleAAtofile(outputWeights,numHiddenNodes,numOutputs,file);
}

//==========OTHER==========

double *init_arr_weighted(int size)
{
    /*
    Retourne un array (pointeur) de taille size.
    Les valeurs sont des poids venant de init_weight().
    */
    double* res = malloc(size * sizeof(double));
    for(int i = 0; i < size; i++)
        res[i] = init_weight();
    return res;
}

double **init_2arr_weighted(int size1, int size2)
{
    /*
    Retourne un array d'array (en pointeurs) de taille size1 * size2.
    Les valeurs sont des poids venant de init_weight().
    */
    double** res = malloc(size1 * sizeof(double));
    for(int i = 0; i < size1; i++)
    {
        res[i] = init_arr_weighted(size2);
    }
    return res;
}

//==========FIN DE MA PARTIE==========


// Simple network that can learn XOR
// Feartures : sigmoid activation function, stochastic gradient descent, and mean square error fuction

// Activation function and its derivative
double sigmoid(double x) { return 1 / (1 + exp(-x)); }
double dSigmoid(double x) { return x * (1 - x); }
// Activation function and its derivative
double init_weight() { return ((double)rand())/((double)RAND_MAX); }

// Shuffle the dataset
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}


void init(double** hiddenWeights, double* hiddenLayerBias, double** outputWeights, double* outputLayerBias){
    for (int i=0; i<numInputs; i++) {
        for (int j=0; j<numHiddenNodes; j++) {
            hiddenWeights[i][j] = init_weight();
        }
    }
    for (int i=0; i<numHiddenNodes; i++) {
        hiddenLayerBias[i] = init_weight();
        for (int j=0; j<numOutputs; j++) {
            outputWeights[i][j] = init_weight();
        }
    }
    for (int i=0; i<numOutputs; i++) {
        outputLayerBias[i] = init_weight();
    }
}


void forwardpass(double training_inputs[numTrainingSets][numInputs], double training_outputs[numTrainingSets][numOutputs], int i, double** hiddenWeights,
                 double* hiddenLayerBias, double* hiddenLayer, double** outputWeights, double* outputLayerBias,
                 double* outputLayer){

    // Compute hidden layer activation
    for (int j=0; j<numHiddenNodes; j++) {
        double activation = hiddenLayerBias[j];
        for (int k=0; k<numInputs; k++) {
            activation += training_inputs[i][k] * hiddenWeights[k][j];
        }
        hiddenLayer[j] = sigmoid(activation);
    }

    // Compute output layer activation
    for (int j=0; j<numOutputs; j++) {
        double activation = outputLayerBias[j];
        for (int k=0; k<numHiddenNodes; k++) {
            activation += hiddenLayer[k] * outputWeights[k][j];
        }
        outputLayer[j] = sigmoid(activation);
    }

    // Print the results from forward pass
    printf ("Input:%g %g   Output:%g    Expected Output: %g\n",
            training_inputs[i][0], training_inputs[i][1],
            outputLayer[0], training_outputs[i][0]);
}



void backprop(double training_inputs[numTrainingSets][numInputs], double training_outputs[numTrainingSets][numOutputs], int i, double** hiddenWeights,
              double* hiddenLayerBias, double* hiddenLayer, double** outputWeights, double* outputLayerBias,
              double* outputLayer, double lr){
    // Compute change in output weights
    double deltaOutput[numOutputs];
    for (int j=0; j<numOutputs; j++) {
        double errorOutput = (training_outputs[i][j] - outputLayer[j]);
        deltaOutput[j] = errorOutput * dSigmoid(outputLayer[j]);
    }

    // Compute change in hidden weights
    double deltaHidden[numHiddenNodes];
    for (int j=0; j<numHiddenNodes; j++) {
        double errorHidden = 0.0f;
        for(int k=0; k<numOutputs; k++) {
            errorHidden += deltaOutput[k] * outputWeights[j][k];
        }
        deltaHidden[j] = errorHidden * dSigmoid(hiddenLayer[j]);
    }

    // Apply change in output weights
    for (int j=0; j<numOutputs; j++) {
        outputLayerBias[j] += deltaOutput[j] * lr;
        for (int k=0; k<numHiddenNodes; k++) {
            outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j] * lr;
        }
    }

    // Apply change in hidden weights
    for (int j=0; j<numHiddenNodes; j++) {
        hiddenLayerBias[j] += deltaHidden[j] * lr;
        for(int k=0; k<numInputs; k++) {
            hiddenWeights[k][j] += training_inputs[i][k] * deltaHidden[j] * lr;
        }
    }
}


void finalprint(double** hiddenWeights, double* hiddenLayerBias, double** outputWeights, double* outputLayerBias){

    fputs ("Final Hidden Weights\n[ ", stdout);
    for (int j=0; j<numHiddenNodes; j++) {
        fputs ("[ ", stdout);
        for(int k=0; k<numInputs; k++) {
            printf ("%f ", hiddenWeights[k][j]);
        }
        fputs ("] ", stdout);
    }

    fputs ("]\nFinal Hidden Biases\n[ ", stdout);
    for (int j=0; j<numHiddenNodes; j++) {
        printf ("%f ", hiddenLayerBias[j]);
    }

    fputs ("]\nFinal Output Weights", stdout);
    for (int j=0; j<numOutputs; j++) {
        fputs ("[ ", stdout);
        for (int k=0; k<numHiddenNodes; k++) {
            printf ("%f ", outputWeights[k][j]);
        }
        fputs ("]\n", stdout);
    }

    fputs ("Final Output Biases\n[ ", stdout);
    for (int j=0; j<numOutputs; j++) {
        printf ("%f ", outputLayerBias[j]);

    }

    fputs ("]\n", stdout);
}


int main(int argc, char **argv){

    const double lr = 0.1f;

    double* hiddenLayer = malloc(numHiddenNodes * sizeof(double));
    double* outputLayer = malloc(numOutputsNodes * sizeof(double));

    double* hiddenLayerBias = malloc(numHiddenNodes * sizeof(double)); // Biais en fonction de la node
    double* outputLayerBias = malloc(numOutputs * sizeof(double));

    double ** hiddenWeights = malloc(numInputs * sizeof(*hiddenWeights));// Weight en fonction nb hidden nodes et nb input
    for (int i = 0; i < numInputs ; ++i) {
        hiddenWeights[i] = malloc(numHiddenNodes * sizeof(*hiddenWeights[i]));
    }
    double** outputWeights = malloc(numHiddenNodes * sizeof(*outputWeights));
    for (int i = 0; i < numHiddenNodes ; ++i) {
        outputWeights[i] = malloc(numOutputs * sizeof(*outputWeights[i]));
    }

    double training_inputs[numTrainingSets][numInputs] = {{0.0f,0.0f},
                                                          {1.0f,0.0f},
                                                          {0.0f,1.0f},
                                                          {1.0f,1.0f}};
    double training_outputs[numTrainingSets][numOutputs] = {{0.0f},
                                                            {1.0f},
                                                            {1.0f},
                                                            {0.0f}};

    init(hiddenWeights, hiddenLayerBias, outputWeights, outputLayerBias);

    int trainingSetOrder[] = {0,1,2,3};

    int numberOfEpochs = 100;
    // Train the neural network for a number of epochs
    for(int epochs=0; epochs < numberOfEpochs; epochs++) {

        // As per SGD, shuffle the order of the training set
        shuffle(trainingSetOrder,numTrainingSets);

        // Cycle through each of the training set elements
        for (int x=0; x<numTrainingSets; x++) {

            int i = trainingSetOrder[x];

            // Forward pass
            forwardpass(training_inputs, training_outputs, i, hiddenWeights, hiddenLayerBias, hiddenLayer,
                        outputWeights, outputLayerBias, outputLayer);

            // Backprop
            backprop(training_inputs, training_outputs, i, hiddenWeights, hiddenLayerBias, hiddenLayer,
                     outputWeights, outputLayerBias, outputLayer, lr);
        }
    }

    // Print final weights after training
    finalprint(hiddenWeights, hiddenLayerBias, outputWeights, outputLayerBias);


    free(hiddenLayerBias);
    free(hiddenWeights);
    free(hiddenLayer);
    free(outputLayerBias);
    free(outputWeights);
    free(outputLayer);

    return 0;
}