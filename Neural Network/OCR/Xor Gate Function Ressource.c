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
}//*/

