#include "NN.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "NNtraining.h"
#include "NNsave.h"

#define nbInput 28*28

#define nbILayer 1
#define nbHLayer 3
#define nbOLayer 1

#define nbHNode 16
#define nbONode 10

double randomizer(){
    return (double) rand() / (RAND_MAX / 3);
}

long getmax(layer* a) {
    double max = a->array[0]->value;
    long max_i = 0;
    for (long i = 0; i < a->length; i++) {
        if(a->array[i]->value > max) {
            max = a->array[i]->value;
            max_i = i;
        }
    }
    return max_i;
}

double LeakyReLU(double z, double alpha){
    return z >= 0 ? z : alpha * z;
}

double Z(neuron neurone, double* input, neuron* previous){
    double res = 0;

    if(input == NULL){
        for (int i = 0; i < neurone.weight_length; i++) {
            res += neurone.weight[i] * previous[i].value;
        }
    }
    else{
        for (int i = 0; i < neurone.weight_length; i++) {
            res += neurone.weight[i] * input[i];
        }
    }

    return res + neurone.bias;
}

double dsLeakyReLU(double z, double alpha) {
    return z >= 0 ? 1 : alpha;
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
    double* res = malloc(sizeof(double) * nbONode);
    for (int i = 0; i < nbONode; i++) {
        res[i] = -1 * (get[i] * log(expected[i])
                + (1 - get[i]) * log(1 - expected[i]));
    }

    return res;
}

double average(double* a){

    double res = 0;
    for (int i = 0; i < nbONode; i++) {
        res += a[i];
    }
    return (1/nbONode) * res;
}

double partial_sum(layer* a, int except){
    double res = 0;
    for (int i = 0; i < nbONode; i++) {
        if(i != except)
            res += a->array[i]->value;
    }
    return res;
}

void output_backprop(network* a, double* get, double* error, double learning_rate){

    //Sum of value
    double sum = 0;
    for (int i = 0; i < nbONode; ++i) {
        sum += get[i];
    }

    //Compute dSoftmax
    double* ds = malloc(a->array[a->length - 1]->length * sizeof(double));

    for (int i = 0; i < a->array[a->length - 1]->length; i++) {
        double ps = partial_sum(a->array[a->length - 1], i);
        ds[i] = dssoftmax(a->array[a->length - 1]->array[i]->value, ps, sum);
    }

    //Compute dW, dB and apply change
    for (int i = 0; i <  a->array[a->length - 1]->length; i++) {

        a->array[a->length - 1]->array[i]->bias -= learning_rate * ds[i] * error[i];
        a->array[a->length - 1]->array[i]->value = ds[i];

        for(int j = 0; j < a->array[a->length - 1]->array[i]->weight_length; j++) {
            a->array[a->length - 1]->array[i]->weight[j] -= learning_rate *
                    (a->array[a->length - 2]->array[j]->value * ds[i] * error[i]); //E * df * value_previous_layer
        }
    }

    free(ds);
}

void hidden_backprop(network* a, double* previous_error, double learning_rate){

    for (int i = a->length - 2; i > 0; i--) {

        //Compute Error
        double* new_error = malloc(a->array[i]->length * sizeof(double));

        for (int j = 0; j < a->array[i]->length; j++) {
            for (int k = 0; k < a->array[i+1]->length; k++) {
                new_error[j] += previous_error[k] * a->array[i+1]->array[k]->value * a->array[i+1]->array[k]->weight[j]; // W_following_layer * E_follow * ds_following(get in value)
            }
        }


        //Apply change
        for (int j = 0; j <  a->array[i]->length; j++) {

            double ds = dsLeakyReLU(a->array[i]->array[j]->value, 0.01);

            a->array[i]->array[j]->bias -= learning_rate * ds * new_error[j];
            a->array[i]->array[j]->value = ds;

            for(int k = 0; k < a->array[i]->array[j]->weight_length; k++) {
                a->array[i]->array[j]->weight[k] -= learning_rate *
                        (ds * new_error[j] * a->array[i - 1]->array[j]->value); //E * df * value_previous_layer
            }
        }
	
//	double *tmp = previous_error;
//        free(tmp);
        previous_error = new_error;
    }
}

void input_backprop(network* a, double* input, double* previous_error, double learning_rate){

    //Compute Error
    double* new_error = malloc(a->array[0]->length * sizeof(double));

    for (int j = 0; j < a->array[0]->length; j++) {
        for (int k = 0; k < a->array[1]->length; k++) {
            new_error[j] += previous_error[k] * a->array[1]->array[k]->value * a->array[1]->array[k]->weight[j]; // W_following_layer * E_follow * ds_following(get in value)
        }
    }


    //Apply change
    for (int j = 0; j <  a->array[0]->length; j++) {

        double ds = dsLeakyReLU(a->array[0]->array[j]->value, 0.01);

        a->array[0]->array[j]->bias -= learning_rate * ds * new_error[j];
        a->array[0]->array[j]->value = ds;

        for(int k = 0; k < a->array[0]->array[j]->weight_length; k++) {
            a->array[0]->array[j]->weight[k] -= learning_rate *
                                                (ds * new_error[j] * input[k]); //E * df * value_previous_layer
        }
    }

}

long forwardpass(network* a, double* input, double learning_rate){

    //Input Layer
    for (int i = 0; i < a->array[0]->length; ++i) {
        a->array[0]->array[i]->value = LeakyReLU(Z(*a->array[0]->array[i], input, NULL), learning_rate);
    }

    //Hidden Layer
    for (int i = 1; i < a->length - 1; ++i) {
        for (int j = 0; j < a->array[i]->length; ++j) {
            a->array[i]->array[j]->value = LeakyReLU(Z(*a->array[i]->array[j], NULL,*a->array[i-1]->array), learning_rate);
        }
    }

    //Output Layer

    //sum for softmax
    double sum = 0;
    for (long i = 0; i < a->array[a->length-1]->length; ++i) {
        sum += exp(Z(*a->array[a->length-1]->array[i],NULL, *a->array[a->length - 2]->array));
    }

    for (long i = 0; i < a->array[a->length - 1]->length; ++i) {
        a->array[a->length - 1]->array[i]->value = softmax(Z(*a->array[a->length-1]->array[i], NULL,
                                                            *a->array[a->length - 2]->array), sum);
    }

    return getmax(a->array[a->length-1]);
}

void backwardpass(network* a, double* get, double* input, double* cost, double learning_rate){

    output_backprop(a, get, cost, learning_rate);
    hidden_backprop(a, cost, learning_rate);
    input_backprop(a, input, cost, learning_rate);
}

void training(network* network, training_image input, long epoch, double learning_rate){

    for (long i = 0; i < epoch; i++) {

        for (size_t j = 0; j < 2; j++) {

            //Forward
            long res = forwardpass(network, input.images[j], learning_rate);

            //Get result softmax int tmp array
            double soft[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            for (int k = 0; k < nbONode; k++) {
                soft[k] = ((network->array[network->length - 1])->array[k])->value;
            }

            //Calc array in tmp format
            double expected[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            expected[(int)input.labels[j]] = 1;

            //Calc error
            double* cost_array = cost(soft, (double *) expected);
            double error = average(cost_array);
            //printf("Enter: %d      Get: %ld       The margin of error: %f\n",
		    //input.labels[j], res, error);

            for (long k = 0; k < network->length; k++) {
                for (long l = 0; l < network->array[k]->length; l++) {
                    printf("Layer: %ld    Neurone: %ld    Value: %f\n", k, l, network->array[k]->array[l]->value);
                    printf("Layer: %ld    Neurone: %ld    Bias: %f\n", k, l, network->array[k]->array[l]->bias);

                    for (int m = 0; m < network->array[k]->array[l]->weight_length; m++) {
                        printf("Layer: %ld    Neurone: %ld    Weight: %f\n", k, l, network->array[k]->array[l]->weight[m]);
                    }
                }
            }

            //Backward
            backwardpass(network, soft, input.images[j], cost_array, learning_rate);

            free(cost_array);
        }
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
    a->array = malloc(sizeof(neuron*) * nb_neuron);
    for (int i = 0; i < nb_neuron; i++) {
        a->array[i] = neuron_init(nb_weight);
    }
    return a;
}

network* initialisation(){

    long nb_layer = nbILayer + nbHLayer + nbOLayer;

    network* a = malloc(sizeof(network));
    a->length = nb_layer;
    a->array = malloc(sizeof(layer*) * (a->length));

    long n = 0;

    //------------------------- I Layer -------------------------
    for (int i = 0; i < nbILayer; i++) {
        a->array[n] = layer_init(nbInput, nbInput);
        n++;
    }

    //------------------------- H Layer -------------------------
    a->array[n] = layer_init(nbHNode, nbInput);
    n++;

    for (int i = 0; i < nbHLayer; i++) {
        a->array[n] = layer_init(nbHNode, nbHNode);
        n++;
    }

    //------------------------- O Layer -------------------------
    for (int i = 0; i < nbOLayer; i++) {
        a->array[n] = layer_init(nbONode, nbHNode);
        n++;
    }

    return a;
}


void free_neuron(neuron* n){
    free(n->weight);
    free(n);
}

void free_layer(layer* l){
    for (int i = 0; i < l->length; i++) {
        free_neuron(l->array[i]);
    }
    free(l->array);
    free(l);
}

void free_network(network* a){
    for (int i = 0; i < a->length; i++) {
        free_layer(a->array[i]);
    }
    free(a->array);
    free(a);
}


int neural_network(){

    double learning_rate = 0.1f;
    network* a = NULL;
    if(!access( "save/Network.txt", F_OK))
        a = LoadNetwork();
    else
        a = initialisation();

    training_image* input = SetupTrainingArrays();
    training(a, *input, 1, learning_rate);
    SaveNetwork(a);
    FreeTrainingArrays(input);
    free_network(a);

    return 0;
}
