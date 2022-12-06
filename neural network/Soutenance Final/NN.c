#include "NN.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "NNtraining.h"
#include "NNsave.h"

#define nbInput 28*28

#define nbILayer 1
#define nbHLayer 1
#define nbOLayer 1

#define nbHNode 16
#define nbONode 10


double he(double previous_layer_size){
    double res = (rand()/(double)RAND_MAX) * sqrt(2/previous_layer_size);
    //printf("%lf\n",res);
    return res;
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

double Z(neuron neurone, double* input, neuron** previous){
    double res = 0;

    if(input == NULL){
        for (int i = 0; i < neurone.weight_length; i++) {
            res += neurone.weight[i] * previous[i]->value;
        }
    }
    else{
        for (int i = 0; i < neurone.weight_length; i++) {
            res += neurone.weight[i] * input[i];
        }
    }

    return res + neurone.bias;
}

double dsLeakyReLU(double z) {
    return z >= 0 ? 1 : 0.01;
}

double softmax(double value, double sum){
    return exp(value) / sum;
}

double partial_sum(layer* a, int except){
    double res = 0;
    for (int i = 0; i < nbONode; i++) {
        if(i != except)
            res += a->array[i]->value;
    }
    return res;
}

//----------------------------NEW FUNCTION----------------------------
double* error_out(layer* layer, double* expected){
    double* sigma = calloc(layer->length, sizeof(double));

    for (int i = 0; i < layer->length; i++) {
        sigma[i] = layer->array[i]->value - expected[i];
    }

    return sigma;
}

double* error_hid(layer* _layer, layer* previous_layer, double* previous_error){

    double* sigma = calloc(_layer->length, sizeof(double));

    for (int i = 0; i < _layer->length; i++) {
        double tmp = 0;
        for (int j = 0; j < previous_layer->length; ++j) {
            tmp += previous_error[j] * previous_layer->array[j]->weight[i];
        }

        tmp *= dsLeakyReLU(_layer->array[i]->value);
        sigma[i] = tmp;
    }
    return sigma;
}

double cross_entropy(layer* layer1, double* expected){
    double res = 0;

    for (int i = 0; i < layer1->length; i++){
        res += expected[i] * log(layer1->array[i]->value) + (1 - expected[i]) * log(1 - layer1->array[i]->value);
    }
    return -1 * res;
}

void back_prop(network* a, double* expected, double learning_rate){ //Back_prop Martin

    double* sigma = NULL;
    double* previous_error = NULL;

    for (int i = a->length-1; i > -1; i--) {

        layer* layer1 = a->array[i];

        if(i == a->length-1){
            sigma = error_out(layer1, expected);
        }
        else{
            if(previous_error != NULL)
                free(previous_error);

            previous_error = sigma;
            sigma = error_hid(layer1, a->array[i-1], previous_error);
        }

        for (int j = 0; j < layer1->length; j++) {
            neuron* neuron1 = layer1->array[j];
            neuron1->bias -= learning_rate * sigma[j];

            for (int k = 0; k < neuron1->weight_length; k++) {
                neuron1->weight[k] -= learning_rate * sigma[j] * neuron1->value;
            }
        }
    }

}

double* dsCross_entropy(layer* layer1, double* expected){

    double* error = calloc(10, sizeof(double));

    for (int i = 0; i < 10; i++) {
        error[i] = -1 * ((expected[i] * (1. / layer1->array[i]->value) + (1 - expected[i]) * (1. / (1 - layer1->array[i]->value))));
    }

    return error;
}

double* dsSoftmax(layer* layer1, double sum){

    double* res = calloc(layer1->length, sizeof(double));
    for (int i = 0; i < layer1->length; i++) {
        res[i] = (exp(layer1->array[i]->value) * partial_sum(layer1, i)) / (sum * sum);
    }

    return res;
}

double error_sum(layer* layer1, double* error, int indexe){
    double res = 0;
    for (int i = 0; i < layer1->length; ++i) {
        res += layer1->array[i]->weight[indexe] * error[i];
    }

    return res;
}

double softsum(layer* layer1){
    double res = 0;

    for (int i = 0; i < layer1->length; ++i) {
        res += layer1->array[i]->value;
    }
    return res;
}

double* error(layer* layer1, layer* next_layer, double* previous_error, char is_output){

    double* res = calloc(layer1->length, sizeof(double));

    if(is_output){
        double sum = softsum(layer1);
        double* dssoft = dsSoftmax(layer1, sum);

        for (int i = 0; i < layer1->length; i++) {
            res[i] = dssoft[i] * (layer1->array[i]->value - previous_error[i]);
        }
    }
    else{
        for (int i = 0; i < layer1->length; i++) {
            res[i] = error_sum(next_layer, previous_error, i) * dsLeakyReLU(layer1->array[i]->value);
        }
    }

    return res;
}

void back_back_prop(network* a, double* input, double* expected, double learning_rate){

    //Output
    layer* layer1 = a->array[2];
    double* sigma = error(layer1, NULL, expected, 1);

    for (int i = 0; i < layer1->length; i++) {

        neuron* neuron1 = layer1->array[i];
        neuron1->bias -= learning_rate * sigma[i];

        layer* layer2 = a->array[1];
        for (int j = 0; j < neuron1->weight_length; j++) {
            neuron1->weight[j] -= learning_rate * sigma[i] * layer2->array[j]->value;
        }
    }

    //Hidden
    layer1 = a->array[1];
    double* previous_error = sigma;
    sigma = error(layer1, a->array[2], previous_error, 0);

    for (int i = 0; i < layer1->length; i++) {

        neuron* neuron1 = layer1->array[i];
        neuron1->bias -= learning_rate * sigma[i];

        layer* layer2 = a->array[0];
        for (int j = 0; j < neuron1->weight_length; j++) {
            neuron1->weight[j] -= learning_rate * sigma[i] * layer2->array[j]->value;
        }
    }

    //Input
    layer1 = a->array[0];
    free(previous_error);
    previous_error = sigma;
    sigma = error(layer1, a->array[1], previous_error, 0);

    for (int i = 0; i < layer1->length; i++) {

        neuron* neuron1 = layer1->array[i];
        neuron1->bias -= learning_rate * sigma[i];

        for (int j = 0; j < neuron1->weight_length; j++) {
            neuron1->weight[j] -= learning_rate * sigma[i] * input[i];
        }
    }

    free(previous_error);
    free(sigma);
}



//----------------------------END NEW FUNCTION----------------------------

void output_backprop(network* a, double* get, double* error, double learning_rate){

    //Compute dSoftmax
    double sum = softsum(a->array[2]);
    double* ds = dsSoftmax(a->array[2], sum);

    //Compute dW, dB and apply change
    for (int i = 0; i <  a->array[a->length - 1]->length; i++) {

        a->array[a->length - 1]->array[i]->bias -= learning_rate * ds[i] * error[i];
        a->array[a->length - 1]->array[i]->value = ds[i] * get[i];
        //if(isnan(a->array[a->length - 1]->array[i]->bias) || isnan(a->array[a->length - 1]->array[i]->value))
        //    printf("Line 122\n");

        for(int j = 0; j < a->array[a->length - 1]->array[i]->weight_length; j++) {
            a->array[a->length - 1]->array[i]->weight[j] -= learning_rate * //HERE
                    (a->array[a->length - 2]->array[j]->value * ds[i] * error[i]); //E * df * value_previous_layer
            //if(isnan(a->array[a->length - 1]->array[i]->weight[j]))
            //    printf("Line 128\n");
        }
    }

    free(ds);
}

void hidden_backprop(network* a, double** previous_error, double learning_rate){

    for (int i = a->length - 2; i > 0; i--) {

        //Compute Error
        double* new_error = calloc(a->array[i]->length,sizeof(double));

        for (int j = 0; j < a->array[i]->length; j++) {
            for (int k = 0; k < a->array[i+1]->length; k++) {
                new_error[j] += (*previous_error)[k] * a->array[i+1]->array[k]->value * a->array[i+1]->array[k]->weight[j]; // W_following_layer * E_follow * ds_following(get in value)
            }
        }


        //Apply change
        for (int j = 0; j <  a->array[i]->length; j++) {

            double ds = dsLeakyReLU(a->array[i]->array[j]->value);

            a->array[i]->array[j]->bias -= learning_rate * ds * new_error[j];
            a->array[i]->array[j]->value = ds; //HERE
            //if(isnan(a->array[i]->array[j]->bias) || isnan(a->array[i]->array[j]->value))
            //    printf("Line 157\n");

            for(int k = 0; k < a->array[i]->array[j]->weight_length; k++) {
                a->array[i]->array[j]->weight[k] -= learning_rate * //HERE
                        (ds * new_error[j] * a->array[i - 1]->array[j]->value); //E * df * value_previous_layer
                //if(isnan(a->array[i]->array[j]->weight[k]))
                //    printf("Line 163\n");
            }
        }
	
//	double *tmp = previous_error;
//        free(tmp);
        //previous_error = new_error;
        *previous_error = realloc(*previous_error,a->array[i]->length * sizeof(double));
        for(long c = 0; c < a->array[i]->length; c++)
        {
            (*previous_error)[c] = new_error[c]; //On recopie new_error dans previous_error
        }
        free(new_error);
    }
}

void input_backprop(network* a, double* input, double* previous_error, double learning_rate){

    //Compute Error
    double* new_error = calloc(a->array[0]->length,sizeof(double));

    for (int j = 0; j < a->array[0]->length; j++) { //Len of inp layer
        for (int k = 0; k < a->array[1]->length; k++) { //Len of first hidden layer
            //printf("%d | %d\n",j,k);
            new_error[j] += previous_error[k] * a->array[1]->array[k]->value * a->array[1]->array[k]->weight[j]; // W_following_layer * E_follow * ds_following(get in value)
        }
    }


    //Apply change
    for (int j = 0; j <  a->array[0]->length; j++) {

        double ds = dsLeakyReLU(a->array[0]->array[j]->value);

        a->array[0]->array[j]->bias -= learning_rate * ds * new_error[j];
        a->array[0]->array[j]->value = ds; //HERE
        //if(isnan(a->array[0]->array[j]->bias) || isnan(a->array[0]->array[j]->value))
        //    printf("Line 200\n");

        for(int k = 0; k < a->array[0]->array[j]->weight_length; k++) {
            a->array[0]->array[j]->weight[k] -= learning_rate * //HERE
                                                (ds * new_error[j] * input[k]); //E * df * value_previous_layer
            //if(isnan(a->array[0]->array[j]->weight[k]))
            //    printf("Line 206 : %lf %lf %lf\n",ds,new_error[j],input[k]);
        }
    }
    free(new_error);
}

long forwardpass(network* a, double* input, double learning_rate){

    //Input Layer
    for (int i = 0; i < a->array[0]->length; i++) {
        a->array[0]->array[i]->value = LeakyReLU(Z(*a->array[0]->array[i], input, NULL), learning_rate); //HERE
        //if(isnan(a->array[0]->array[i]->value))
        //    printf("Line 218\n");
    }
        /*for (long l = 0; l < a->array[0]->length; l++) {

            printf("Layer: %ld    Neurone: %ld    Value: %f\n", 0, l, a->array[0]->array[l]->value);
            printf("Layer: %ld    Neurone: %ld    Bias: %f\n", 0, l, a->array[0]->array[l]->bias);

            for (int m = 0; m < a->array[0]->array[l]->weight_length; m++) {
                printf("Layer: %ld    Neurone: %ld    Weight: %f\n", 0, l, a->array[0]->array[l]->weight[m]);
            }
        }*/

    //Hidden Layer
    for (int i = 1; i < a->length - 1; i++) { //pb i
        for (int j = 0; j < a->array[i]->length; ++j) {
            a->array[i]->array[j]->value = LeakyReLU(Z(*a->array[i]->array[j], NULL, a->array[i - 1]->array), learning_rate); //HERE
            //if(isnan(a->array[i]->array[j]->value))
            //    printf("Line 235\n");
        }

        /*for (long l = 0; l < a->array[0]->length; l++) {

            printf("Layer: %ld    Neurone: %ld    Value: %f\n", 0, l, a->array[i]->array[l]->value);
            printf("Layer: %ld    Neurone: %ld    Bias: %f\n", i, l, a->array[i]->array[l]->bias);

            for (int m = 0; m < a->array[0]->array[l]->weight_length; m++) {
                printf("Layer: %ld    Neurone: %ld    Weight: %f\n", i, l, a->array[i]->array[l]->weight[m]);
            }
        }*/
    }


    //Output Layer

    //sum for softmax
    double sum = 0;
    for (long i = 0; i < a->array[a->length-1]->length; i++) {
        a->array[a->length-1]->array[i]->value = Z(*a->array[a->length-1]->array[i],NULL, a->array[a->length - 2]->array);
        sum += exp(a->array[a->length-1]->array[i]->value);
    }

    for (long i = 0; i < a->array[a->length - 1]->length; i++) {
        a->array[a->length - 1]->array[i]->value = softmax(a->array[a->length - 1]->array[i]->value, sum);
        //if(isnan(a->array[a->length - 1]->array[i]->value))
        //    printf("Line 262 : %lf %lf\n",sum,a->array[a->length - 1]->array[i]->value);
    }

    /*for (long l = 0; l < a->array[a->length - 1]->length; l++) {

        printf("Layer: %ld    Neurone: %ld    Value: %f\n", a->length - 1, l, a->array[a->length - 1]->array[l]->value);
        printf("Layer: %ld    Neurone: %ld    Bias: %f\n", a->length - 1, l, a->array[a->length - 1]->array[l]->bias);

        for (int m = 0; m < a->array[0]->array[l]->weight_length; m++) {
            printf("Layer: %ld    Neurone: %ld    Weight: %f\n", a->length - 1, l, a->array[a->length - 1]->array[l]->weight[m]);
        }
    }*/

    return getmax(a->array[a->length-1]);
}

void backwardpass(network* a, double* get, double* input, double** cost, double learning_rate){
    output_backprop(a, get, *cost, learning_rate);
    hidden_backprop(a, cost, learning_rate);
    input_backprop(a, input, *cost, learning_rate);
}

void training(network* network, training_image input, long epoch, double learning_rate){

    for (long i = 0; i < epoch; i++) {

        for (size_t j = 0; j < 1; j++) {

            //Forward
            long res = forwardpass(network, input.images[j], learning_rate);

            //Calc array in tmp format
            double expected[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            expected[(int)input.labels[j]] = 1;

            //Calc error
            double error = cross_entropy(network->array[network->length - 1], expected);
            printf("Enter: %d      Get: %ld       The margin of error: %f\n",
		    input.labels[j], res, error);

            //Backward
            back_back_prop(network, input.images[j], expected,learning_rate);
        }
    }
}

neuron* neuron_init(long nb_weight, long previous_layer_size){
    neuron* a = malloc(sizeof(neuron));
    a->value = 0;
    a->bias = he((double) previous_layer_size);
    a->weight_length = nb_weight;
    a->weight = calloc(nb_weight,sizeof(double));
    for(int k = 0; k < nb_weight; k++) {
        a->weight[k] = he((double) previous_layer_size);
    }
    return a;
}

layer* layer_init(long nb_neuron, long nb_weight, long previous_layer_size){
    layer* a = malloc(sizeof(layer));
    a->length = nb_neuron;
    a->array = malloc(sizeof(neuron*) * nb_neuron);
    for (int i = 0; i < nb_neuron; i++) {
        a->array[i] = neuron_init(nb_weight, previous_layer_size);
    }
    return a;
}

network* initialisation(){

    network* a = malloc(sizeof(network));
    a->length = 3;
    a->array = malloc(sizeof(layer*) * (a->length));

    a->array[0] = layer_init(nbInput, nbInput, nbInput);

    a->array[1] = layer_init(nbHNode, nbInput, a->array[0]->length);

    a->array[2] = layer_init(nbONode, nbHNode, a->array[1]->length);

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
    //SaveNetwork(a);

    training_image* input = SetupTrainingArrays();
    training(a, *input, 1, learning_rate);
    //SaveNetwork(a);
    FreeTrainingArrays(input);
    free_network(a);

    return 0;
}