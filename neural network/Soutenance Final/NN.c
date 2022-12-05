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

#define small_constant 0.000000001

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
        double logex = expected[i] == 0 ? -INFINITY : log(expected[i]);
        double lognex = expected[i] == 1 ? INFINITY : log(1-expected[i]);
        res[i] = -1 * (get[i] * logex)
                + (1 - get[i]) * lognex; //TODO : fix log(0) error
        //printf("%lf %lf\n",get[i],res[i]);
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

        a->array[a->length - 1]->array[i]->bias -= learning_rate * ds[i] * error[i]; //HERE
        a->array[a->length - 1]->array[i]->value = ds[i];
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

            double ds = dsLeakyReLU(a->array[i]->array[j]->value, 0.01);

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

        double ds = dsLeakyReLU(a->array[0]->array[j]->value, 0.01);

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
        if(isnan(a->array[0]->array[i]->value))
            printf("Line 218\n");
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
            if(isnan(a->array[i]->array[j]->value))
                printf("Line 235\n");
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
        sum += exp(Z(*a->array[a->length-1]->array[i],NULL, a->array[a->length - 2]->array));
    }

    for (long i = 0; i < a->array[a->length - 1]->length; i++) {
        a->array[a->length - 1]->array[i]->value = softmax(Z(*a->array[a->length-1]->array[i], NULL, //HERE
                                                            a->array[a->length - 2]->array), sum);
        if(isnan(a->array[a->length - 1]->array[i]->value))
            printf("Line 262 : %lf %lf\n",sum,a->array[a->length - 1]->array[i]->value);
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
    //print_array(*cost,nbONode);
    output_backprop(a, get, *cost, learning_rate);
    //print_array(*cost,nbONode);
    hidden_backprop(a, cost, learning_rate);
    //print_array(*cost,nbHNode);
    input_backprop(a, input, *cost, learning_rate);
    //print_array(*cost,nbHNode);
}

void training(network* network, training_image input, long epoch, double learning_rate){

    for (long i = 0; i < epoch; i++) {

        for (size_t j = 0; j < 1; j++) {

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
            double* cost_array = cost(soft, (double *) expected); //size = 10
            double error = average(cost_array);
            printf("Enter: %d      Get: %ld       The margin of error: %f\n",
		    input.labels[j], res, error);

            //Backward
            backwardpass(network, soft, input.images[j], &cost_array, learning_rate);

            free(cost_array);
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

    long nb_layer = nbILayer + nbHLayer + nbOLayer;

    network* a = malloc(sizeof(network));
    a->length = nb_layer;
    a->array = malloc(sizeof(layer*) * (a->length));


    a->array[0] = layer_init(nbInput, nbInput, nbInput);

    a->array[1] = layer_init(nbHNode, nbInput, a->array[0]->length);

    for (int i = 2; i < nbHLayer + 1; i++) {
        a->array[i] = layer_init(nbHNode, nbHNode, a->array[i-1]->length);
    }

    a->array[nb_layer - 1] = layer_init(nbONode, nbHNode, a->array[nb_layer -2]->length);

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

    srand(time(NULL));

    training_image* input = SetupTrainingArrays();
    training(a, *input, 1, learning_rate);
    SaveNetwork(a);
    FreeTrainingArrays(input);
    free_network(a);

    return 0;
}
// Trouver le 0 originel