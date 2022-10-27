//
// Created by ppaza on 03/09/2022.
// NN that learn xor from https://www.youtube.com/watch?v=LA4I3cWkp1E
// and github link : https://github.com/niconielsen32/NeuralNetworks/blob/b5b6ea70c3036531b6e2fe23a4b5e112db1c69ae/neuralNetC.c
//Note : utiliser les matrices pour machine learnia
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

#define numInputs 2
#define numHiddenNodes 2
#define numOutputs 1
#define numTrainingSets 4

const double learning_rate = 0.1f;

    double hiddenLayer[numHiddenNodes];
    double outputLayer[numOutputs];

    double hiddenLayerBias[numHiddenNodes];
    double outputLayerBias[numOutputs];

    double hiddenWeights[numInputs][numHiddenNodes];
    double outputWeights[numHiddenNodes][numOutputs];


    double training_inputs[numTrainingSets][numInputs] = {{0.0f,0.0f},
                                                          {1.0f,0.0f},
                                                          {0.0f,1.0f},
                                                          {1.0f,1.0f}};
    double training_outputs[numTrainingSets][numOutputs] = {{0.0f},
                                                            {1.0f},
                                                            {1.0f},
                                                            {0.0f}};

//==========Partie Save and Load==========

void print_array(double arr[],int size)
{
    printf("[");
    for(int i = 0; i < size; i++)
    {
        printf("%f",*(arr + i));
        if(i < size - 1)
            printf(", ");
    }
    printf("]\n");
}

char readnchars(FILE *file,unsigned int n)
{
    unsigned int i = 0;
    while(i < n && !feof(file))
    {
        fgetc(file);
    }
    return i < n;
}

double doublefromfile(FILE *file)
{
    double res = 0;
    fscanf(file,"%lf",&res);
    return res;
}

char doubleAfromfile(FILE *file,int size,double array[])
{
    /*
    Fills an array of size 'size' with doubles gotten from the file 'file'.
    Returns 1 if an error occured, 0 else
    */

    fgetc(file); // {
    fgetc(file); // \n
    int i = 0;
    while(!feof(file) && i < size)
    {
        array[i] = doublefromfile(file);
        fgetc(file); //Le \n
        i++;
    }
    fgetc(file); // }
    fgetc(file); // \n
    return i < size;
}

char doubleAtofile(FILE *file,int size,double array[])
{
    /*
    Fills an array of size 'size' with doubles gotten from the file 'file'.
    Returns 1 if an error occured, 0 else
    */
    fprintf(file,"%s","{\n");
    int i = 0;
    while(!feof(file) && i < size)
    {
        fprintf(file,"%f",array[i]);
        fprintf(file,"%c",'\n');
        i++;
    }
    fprintf(file,"%s","}\n");
    return i < size;
}

char loadhiddenWeight(FILE *file)
{
    readnchars(file,2); // '[\n'
    int i = 0;
    char res = 0;
    while(!feof(file) && !res && i < numInputs)
    {
        res = doubleAfromfile(file,numHiddenNodes,hiddenWeight[i]);
        i++;
    }
    res = res || readnchars(file,2); // ']\n;
    return res || i < numInputs;
}
char savehiddenWeight(FILE *file)
{
    fprintf(file,"%s","[\n");
    int i = 0;
    char res = 0;
    while(!feof(file) && !res && i < numInputs)
    {
        res = doubleAtofile(file,numHiddenNodes,hiddenWeight[i]);
        i++;
    }
    fprintf(file,"%s","]\n");
    return res || i < numInputs;
}

char loadoutputWeight(FILE *file)
{
    readnchars(file,2); // '[\n'
    int i = 0;
    char res = 0;
    while(!feof(file) && !res && i < numHiddenNodes)
    {
        res = doubleAfromfile(file,numOutputNodes,outputWeight[i]);
        i++;
    }
    res = res || readnchars(file,2); // ']\n'
    return res || i < numOutputNodes;
}
char saveoutputWeight(FILE *file)
{
    fprintf(file,"%s","[\n");
    int i = 0;
    char res = 0;
    while(!feof(file) && !res && i < numHiddenNodes)
    {
        res = doubleAtofile(file,numOutputNodes,outputWeight[i]);
        i++;
    }
    fprintf(file,"%s","]\n");
    return res || i < numOutputNodes;
}

char load_XOR(char *path)
{
   // Error code = 1
   // All good code = 0
    FILE *file = fopen(path,"r");
    if(file == NULL)
    {
        printf("Bad path given to load XOR.\n");
        return 1; //Error code
    }
    char res = 0;

    res = res || doubleAfromfile(file,numHiddenNodes,hiddenLayer);
    res = res || doubleAfromfile(file,numOutputNodes,outputLayer);
    res = res || doubleAfromfile(file,numHiddenNodes,hiddenLayerBias);
    res = res || doubleAfromfile(file,numOutputNodes,outputLayerBias);
    res = res || loadhiddenWeight(file);
    res = res || loadoutputWeight(file);
    
    return res;
}
char save_XOR(char *path)
{
   // Error code = 1
   // All good code = 0
    FILE *file = fopen(path,"w");
    if(file == NULL)
    {
        printf("Bad path given to save XOR.\n");
        return 1; //Error code
    }
    char res = 0;

    res = res || doubleAtofile(file,numHiddenNodes,hiddenLayer);
    res = res || doubleAtofile(file,numOutputNodes,outputLayer);
    res = res || doubleAtofile(file,numHiddenNodes,hiddenLayerBias);
    res = res || doubleAtofile(file,numOutputNodes,outputLayerBias);
    res = res || savehiddenWeight(file);
    res = res || saveoutputWeight(file);
    
    return res;
}

//==========Fin de partie Save and Load==========



int main(void) // Remplacer LayerBias par vecteurs !
{
    

    for (int i = 0; i < numInputs; ++i) {
        for (int j = 0; j < numHiddenNodes; ++j) {
            hiddenWeight[i][j] = init_weights();
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

    int trainingSetOrder[] = {0,1,2,3};

    int numberOfEpochs = 10000;
    // Train the neural network for a number of epochs
    for(int epochs=0; epochs < numberOfEpochs; epochs++) {

        // As per SGD, shuffle the order of the training set
        shuffle(trainingSetOrder,numTrainingSets);

        // Cycle through each of the training set elements
        for (int x=0; x<numTrainingSets; x++) {

            int i = trainingSetOrder[x];

            // Forward pass

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
            printf ("Input:%g  Output:%g    Expected Output: %g\n",
                    training_inputs[i][0], training_inputs[i][1],
                    outputLayer[0], training_outputs[i][0]);



            // Backprop

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
    }

    // Print final weights after training
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

    return 0;
}//*/

