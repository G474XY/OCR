#include "NN.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define nbInput 28*28

#define nbILayer 1
#define nbHLayer 3
#define nbOLayer 1

#define nbHNode1 784
#define nbHNode2 392
#define nbHNode3 196
#define nbONode 1

#define MAX(x,y) ((x)>(y)?(x):(y))

double getmax(double* a, int len) {
    double max = a[0];
    for (int i = 0; i < a; i++) {
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
    double res[len]; //Plutot un malloc nan ?
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

void forwardpass(){}

void backwardpass(){}

void shuffle(){}

layer* init(){
    layer* a = malloc(sizeof(layer) * (nbILayer + nbHLayer + nbOLayer));
    long n = 0;

    for (long j = 0; j < nbInput; ++j) {
        a[n].array[j].bias = 0; //random value ---------------------------------------------
        a[n].array[j].value = 0;
        a[n].array[j].weight = malloc(sizeof(double) * nbInput);
        for (long k = 0; k < nbInput; ++k) {
            a[n].array[j].weight[k] = 0; //random value ---------------------------------------------
        }
    }
    n++;

    //------------------------------- 1e couche hidden -------------------------------
    a[n].length = 0;
    a[n].array = malloc(sizeof(neuron) * nbHNode1);

    for (long j = 0; j < nbHNode1; ++j) {
        a[n].array[j].bias = 0; //random value ---------------------------------------------
        a[n].array[j].value = 0;
        a[n].array[j].weight = malloc(sizeof(double) * nbInput);
        for (long k = 0; k < nbInput; ++k) {
            a[n].array[j].weight[k] = 0; //random value ---------------------------------------------
        }
    }
    n++;

    for (long i = 1; i < nbHLayer; ++i) {
        a[n].length = 0;
        a[n].array = malloc(sizeof(neuron) * nbHNode1);
        for (long j = 0; j < nbInput; ++j) {
            a[n].array[j].bias = 0; //random value ---------------------------------------------
            a[n].array[j].value = 0;
            a[n].array[j].weight = malloc(sizeof(double) * nbInput);
            for (long k = 0; k < nbInput; ++k) {
                a[n].array[j].weight[k] = 0; //random value ---------------------------------------------
            }
        }
        n++;
    }

    //------------------------------- couche output -------------------------------

    a[n].length = 0;
    a[n].array = malloc(sizeof(neuron) * nbONode);

    a[n].array[0].bias = 0; //random value ---------------------------------------------
    a[n].array[0].value = 0;
    a[n].array[0].weight = malloc(sizeof(double) * nbHNode1);
    for (long k = 0; k < nbInput; ++k) {
        a[n].array[0].weight[k] = 0; //random value ---------------------------------------------
    }

    return a;
}

int main(){

    //const double learning_rate = 0.1f;
    layer * a = init();
    printf("Hello World");

    return 0;
}