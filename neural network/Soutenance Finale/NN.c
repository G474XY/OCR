#include "NN.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define nbInput 28*28

#define nbILayer 1
#define nbHLayer 3
#define nbOLayer 1

#define nbHNode 784
#define nbONode 1

#define MAX(x,y) ((x)>(y)?(x):(y))

double getmax(double* a, int len) {
    double max = a[0];
    for (int i = 0; i < len; i++) {
        max = a[i] > max ? a[i] : max;
    }
    return max;
}

void LeakyReLU(neuron *neuron, double z, double alpha) {
    neuron->value = MAX(alpha * z, z);
}

void dsLeakyReLU(neuron *neuron, double z, double alpha) {
    neuron->value = z > 0 ? 1 : alpha;
}

double* softmax(double* input, int len){
    double* res = malloc(sizeof(double) * len);
    double t = getmax(input, len);

    for (int i = 0; i < len; i++) {
        res[i] = exp(input[i] - t);
    }

    double sumexp = 0;
    for (int i = 0; i < len; ++i) {
        sumexp += res[i];
    }

    for (int i = 0; i < len; ++i) {
        res[i] = res[i] / sumexp;
    }

    return res;
}

void dssoftmax(){}

void forwardpass(layer* a){

}

void backwardpass(){}

void shuffle(){}

neuron neuron_init(long nb_weight){
    neuron* a = malloc(sizeof(neuron));
    a->value = 0;
    a->bias = 1;//Random--------------------------------------------------
    a->weight_length = nb_weight;
    a->weight = malloc(sizeof(double) * nb_weight);
    for(int k = 0; k < nb_weight; k++) {
        a->weight[k] = 1;//Random--------------------------------------------------
    }
    return *a;
}

layer layer_init(long nb_neuron, long nb_weight){
    layer* a = malloc(sizeof(layer));
    a->length = nb_neuron;
    a->array = malloc(sizeof(neuron) * nb_neuron);
    for (int i = 0; i < nb_neuron; i++) {
        a->array[i] = neuron_init(nb_weight);
    }
    return *a;
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

int main(){

    //const double learning_rate = 0.1f;
    network a = initialisation();

    return 0;
}