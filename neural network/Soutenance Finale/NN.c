#include "NN.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define nbInput 28*28

#define nbILayer 1
#define nbHLayer 3
#define nbOLayer 1

#define nbHNode 16
#define nbONode 10

#define MAX(x,y) ((x)>(y)?(x):(y))

double getmax1(neuron* a, int len) {
    double max = a[0].value;
    for (int i = 0; i < len; i++) {
        max = a[i].value > max ? a[i].value : max;
    }
    return max;
}

double getmax2(double* a, int len) {
    double max = a[0];
    for (int i = 0; i < len; i++) {
        max = a[i] > max ? a[i] : max;
    }
    return max;
}

double LeakyReLU(double z, double alpha){
    return MAX(alpha * z, z);
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

void dsLeakyReLU(neuron *neuron, double z, double alpha) {
    neuron->value = z > 0 ? 1 : alpha;
}

double* softmax(neuron* input, int len){
    double* res = malloc(sizeof(double) * len);

    double t = getmax1(input, len);

    for (int i = 0; i < len; i++) {
        res[i] = exp(input[i].value - t);
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

//Log Loss cost function
double cost(double* get, double* expected){
    double res = 0;
    for (int i = 0; i < nbONode; ++i) {
        res += get[i] * log(expected[i]) + (1 - get[i]) * log(1 - expected[i]);
    }
    res *= (1./nbONode);
    return res;
}

double dssoftmax(double* array, int indice, double sum){ //result array of softmax function and indice of the current calc value from softmax too

    double partial_sum = 0;
    for (int i = 0; i < nbONode; ++i) {
        if(i != indice)
            partial_sum += array[i];
    }

    return (array[indice] * partial_sum) / pow(sum, 2);
}


void forwardpass(network a, double* input, double learning_rate){

    //Input Layer
    for (int i = 0; i < a.array[0]->length; ++i) {
        a.array[0]->array[i]->value = LeakyReLU(Z2(*a.array[0]->array[i], input), learning_rate);
    }

    //Hidden Layer
    for (int i = 1; i < a.length-1; ++i) {
        for (int j = 0; j < a.array[i]->length; ++j) {
            a.array[i]->array[j]->value = LeakyReLU(Z1(*a.array[i]->array[j], *a.array[i-1]->array), learning_rate);
        }
    }

    //Output Layer //Pb là
    a.array[a.length-1]->array[0]->value = getmax2(
            softmax(*a.array[a.length-1]->array, a.array[a.length-1]->length), a.array[a.length-1]->length);

}

void backwardpass(network a, double* get, double* expected){

    //Output Layer

    double error = cost(get,expected);

    //Get the sum
    double sum = 0;
    for (int i = 0; i < nbONode; ++i) {
            sum += get[i];
    }

    for (int i = 0; i < a.array[a.length-1]->length; ++i) {

        a.array[a.length-1]->array[i]->value = 0;//qqchose

        for (long j = 0; j < a.array[a.length-1]->array[i]->weight_length; ++j) {
            a.array[a.length-1]->array[i]->weight[j] = 0; //qqchose
        }

        a.array[a.length-1]->array[i]->bias = 0;//qqchose
    }

    //Hidden Layer -- même chose que ci dessus


}

void shuffle(){}

neuron* neuron_init(long nb_weight){
    neuron* a = malloc(sizeof(neuron));
    a->value = 0;
    a->bias = 1;//Random--------------------------------------------------
    a->weight_length = nb_weight;
    a->weight = malloc(sizeof(double) * nb_weight);
    for(int k = 0; k < nb_weight; k++) {
        a->weight[k] = 1;//Random--------------------------------------------------
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

int main(){

    const double learning_rate = 0.1f;
    network a = initialisation();

    return 0;
}