#include "NN.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define nbInput 28*28

#define nbILayer 1
#define nbHLayer 3
#define nbOLayer 1

#define nbHNode 16
#define nbONode 10

int randomizer(){
    return ((double)rand())/((double)RAND_MAX);
}

double getmax(layer* a) {
    double max = a->array[0]->value;
    for (long i = 0; i < a->length; i++) {
        max = a->array[i]->value > max ? a->array[i]->value : max;
    }
    return max;
}

double LeakyReLU(double z, double alpha){
    return alpha * z > z ? alpha * z : z;
}

double Z1(neuron neurone, neuron* input){
    double res = 0;

    for (int i = 0; i < neurone.weight_length; i++) {
            res += neurone.weight[i] * input[i].value;
    }

    return res + neurone.bias;
}

double Z2(neuron neuron, double* input){
    double res = 0;

    for (int i = 0; i < neuron.weight_length; i++) {
        res += neuron.weight[i] * input[i];
    }

    return res + neuron.bias;
}

double dsLeakyReLU(double z, double alpha) {
    return z > 0 ? 1 : alpha;
}

double softmax(double value, double sum){
    return exp(value) / sum;
}

//Result array of softmax function and indice of the current calc value from softmax too
double dssoftmax(double a, double partial_sum, double sum){

    return (a * partial_sum) / (sum * sum);
}

//Log Loss cost function
double* cost(double* get, double* expected){
    double res[nbONode];
    for (int i = 0; i < nbONode; ++i) {
        res[i] = -1 * (get[i] * log(expected[i])
                + (1 - get[i]) * log(1 - expected[i]));
    }

    return res;
}

double average(double* a){

    double res = 0;
    for (int i = 0; i < nbONode; ++i) {
        res += a[i];
    }
    return (1/nbONode) * res;
}

double partial_sum(layer* a, int except){
    double res = 0;
    for (int i = 0; i < nbONode; ++i) {
        if(i != except)
            res += a->array[i]->value;
    }
    return res;
}

double forwardpass(network a, double* input, double learning_rate){

    //Input Layer
    for (int i = 0; i < a.array[0]->length; ++i) {
        a.array[0]->array[i]->value = LeakyReLU(Z2(*a.array[0]->array[i], input), learning_rate);
    }

    //Hidden Layer
    for (int i = 1; i < a.length - 1; ++i) {
        for (int j = 0; j < a.array[i]->length; ++j) {
            a.array[i]->array[j]->value = LeakyReLU(Z1(*a.array[i]->array[j], *a.array[i-1]->array), learning_rate);
        }
    }

    //Output Layer

    //sum for softmax
    double sum = 0;
    for (long i = 0; i < a.array[a.length-1]->length; ++i) {
        sum += exp(Z1(*a.array[a.length-1]->array[i], *a.array[a.length - 2]->array));
    }

    for (long i = 0; i < a.array[i - 1]->length; ++i) {
        a.array[a.length - 1]->array[i]->value = softmax(Z1(*a.array[a.length-1]->array[i],
                                                            *a.array[a.length - 2]->array), sum);
    }

    return getmax(a.array[a.length-1]);
}

void backwardpass(network a, double* get, double* expected, double* cost, double learning_rate){
    
    //Sum of value
    double sum = 0;
    for (int i = 0; i < nbONode; ++i) {
        sum += get[i];
    }

    //Output Layer

    //Compute dSoftmax
    double* ds = malloc(a.array[a.length - 1]->length * sizeof(double));
    for (int i = 0; i < a.array[a.length - 1]->length; i++) {
        double ps = partial_sum(a.array[a.length - 1], i);
        ds[i] = dssoftmax(a.array[a.length - 1]->array[i]->value, ps, sum);
    }

    //Compute dW, dB and apply change
    for (int i = 0; i <  a.array[a.length - 1]->length; ++i) {

        a.array[a.length - 1]->array[i]->bias -= learning_rate * ds[i] * cost[i];
        a.array[a.length - 1]->array[i]->value = ds[i];

        for(int j = 0; j < a.array[a.length - 1]->array[i]->weight_length; ++j) {
            a.array[a.length - 1]->array[i]->weight[j] -= learning_rate *
                    (a.array[a.length - 2]->array[j]->value * ds[i] * cost[i]);
        }
    }

    //Hidden Layer

    for (int i = a.length - 2; i > 0; i--) {

        //Compute dW, dB and apply change
        double* E = malloc(a.array[i]->length);

        for (int j = 0; j < a.array[i]->length; ++j) {
            for (int k = 0; k < a.array[i+1]->length; ++k) {
                E[j] += cost[k] * a.array[i+1]->array[k]->value * a.array[i+1]->array[k]->weight[j];
            }
        }

        //Apply change
        for (int j = 0; j <  a.array[i]->length; j++) {

            a.array[i]->array[j]->bias -= learning_rate * dsLeakyReLU(a.array[i]->array[j]->value, 0.01) * E[j];

            for(int k = 0; k < a.array[i]->array[j]->weight_length; k++) {
                a.array[i]->array[j]->weight[k] -= learning_rate *
                        (dsLeakyReLU(a.array[i]->array[j]->value, 0.01) * E[j] * a.array[i - 1]->array[j]->value);
            }
        }

    }

    //Input Layer



}

void training(network* network, double*** input, long epoch, double learning_rate){

    double** expected[10][10] = {
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
                                 };

    for (long i = 0; i < epoch; ++i) {

        //Choose the input
        int indexe = rand() % 10; // Sélection de la valeur
        int indexe2 = rand() % 10; // Sélection de la valeur dans la liste des représentations possibles

        //Forward
        double res = forwardpass(*network, input[indexe][indexe2], learning_rate);

        //Get result softmax int tmp array
        double tmp[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        for (int j = 0; j < nbONode; ++j) {
            tmp[i] = network->array[network->length - 1]->array[i]->value;
        }

        //Calc error
        double* c = cost(tmp, (double *) expected[indexe]);
        double error = average(c);
        printf("Enter: %d      Get: %f       The margin of error: %f\n", ((indexe + 1) % 10), res, (1 - error * 100));

        //Backward
        backwardpass(*network, tmp, (double *)expected[indexe], error, learning_rate);
    }

}



neuron* neuron_init(long nb_weight){
    neuron* a = malloc(sizeof(neuron));
    a->value = 0;
    a->bias = randomizer();
    a->weight_length = nb_weight;
    a->weight = malloc(sizeof(double) * nb_weight);
    for(int k = 0; k < nb_weight; k++) {
        a->weight[k] = randomizer();
    }
    return a;
}

layer* layer_init(long nb_neuron, long nb_weight){
    layer* a = malloc(sizeof(layer));
    a->length = nb_neuron;
    a->array = malloc(sizeof(neuron) * nb_neuron);
    for (int i = 0; i < nb_neuron; i++) {
        a->array[i] = neuron_init(nb_weight);
    }
    return a;
}

network initialisation(){

    long nb_layer = nbILayer + nbHLayer + nbOLayer;

    network* a = malloc(sizeof(network) * nb_layer);
    a->length = nb_layer;
    a->array = malloc(sizeof(layer) * a->length);

    long n = 0;

    //------------------------- I Layer -------------------------
    for (int i = 0; i < nbILayer; i++) {
        a->array[n] = layer_init(nbInput, nbInput);
        n++;
    }

    //------------------------- H Layer -------------------------
    a->array[n] = layer_init(nbHNode, nbInput);
    n++;

    for (int i = 1; i < nbHLayer; i++) {
        a->array[n] = layer_init(nbHNode, nbHNode);
        n++;
    }

    //------------------------- O Layer -------------------------
    for (int i = 0; i < nbOLayer; i++) {
        a->array[n] = layer_init(nbONode, nbHNode);
        n++;
    }

    return *a;
}

void free_network(network* a){}


int main(int argc, char **argv){

    const double learning_rate = 0.1f;
    network a = initialisation();

    if(strcmp(argv[1], "-t") == 0){ // Check if test mode
        double*** input; // Init training image
        training(&a, input, 100, 0.01);
        // Save the training
    }
    else {
        double* input; // Get image
        double res = forwardpass(a, input, 0.01);
        printf("Result: %f", res);
    }

    free_network(&a);

    return 0;
}