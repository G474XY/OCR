#include "NN.h"
#include "NNtraining.h"
#include "NNsave.h"

#define nbInput 28*28

#define nbILayer 1
#define nbHLayer 1
#define nbOLayer 1

#define nbHNode 16
#define nbONode 10


double randfrom(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

int getmax(layer* a) {
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

double LeakyReLU(double z){
    return z >= 0 ? z : 0.01 * z;
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
    return z >= 0 ? 1 : 0;
}

double cross_entropy(layer* p_layer, double* expected){
    double res = 0;

    for (int i = 0; i < p_layer->length; ++i) {
        res -=  expected[i]* log(p_layer->array[i]->value);
    }

    return res;
}

double partial_sum(layer* a, int except){
    double res = 0;
    for (int i = 0; i < nbONode; i++) {
        if(i != except)
            res += a->array[i]->value;
    }
    return res;
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

void back_prop(network* a, double* input, double* expected, double learning_rate){

    //Output
    layer* p_layer = a->array[2];
    layer* previous_layer = a->array[1];
    double* sigma = error(p_layer, NULL, expected, 1);

    for (int i = 0; i < p_layer->length; i++) {

        neuron* neuron1 = p_layer->array[i];
        neuron1->bias += learning_rate * sigma[i];

        for (int j = 0; j < neuron1->weight_length; j++) {
            neuron1->weight[j] += learning_rate * sigma[i] * previous_layer->array[j]->value;
        }
    }

    //Hidden
    p_layer = a->array[1];
    previous_layer = a->array[0];
    double* previous_error = sigma;
    sigma = error(p_layer, a->array[2], previous_error, 0);

    for (int i = 0; i < p_layer->length; i++) {

        neuron* neuron1 = p_layer->array[i];
        neuron1->bias += learning_rate * sigma[i];

        for (int j = 0; j < neuron1->weight_length; j++) {
            neuron1->weight[j] += learning_rate * sigma[i] * previous_layer->array[j]->value;
        }
    }

    //Input
    p_layer = a->array[0];
    free(previous_error);
    previous_error = sigma;
    sigma = error(p_layer, a->array[1], previous_error, 0);

    for (int i = 0; i < p_layer->length; i++) {

        neuron* neuron1 = p_layer->array[i];
        neuron1->bias += learning_rate * sigma[i];

        for (int j = 0; j < neuron1->weight_length; j++) {
            neuron1->weight[j] += learning_rate * sigma[i] * input[i];
        }
    }

    free(previous_error);
    free(sigma);
}

layer* forwardpass(network* a, double* input) {

    //Input
    layer* p_layer = a->array[0];
    neuron* p_neuron = NULL;

    for (int i = 0; i < p_layer->length; ++i) {
        p_neuron = p_layer->array[i];
        p_neuron->value = LeakyReLU(Z(*p_neuron, input, NULL));
    }

    //Hidden
    layer* p_previous_layer = p_layer;
    p_layer = a->array[1];

    for (int i = 0; i < p_layer->length; ++i) {
        p_neuron = p_layer->array[i];
        p_neuron->value = LeakyReLU(Z(*p_neuron, NULL, p_previous_layer->array));
    }

    //Output
    p_previous_layer = p_layer;
    p_layer = a->array[2];

    double sum_soft = 0;

    for (int i = 0; i < p_layer->length; ++i) {
        p_neuron = p_layer->array[i];
        p_neuron->value = exp(Z(*p_neuron, NULL, p_previous_layer->array));
        sum_soft += p_neuron->value;
    }

    for (int i = 0; i < p_layer->length; ++i) {
        p_neuron = p_layer->array[i];
        p_neuron->value /= sum_soft;
    }

    return p_layer;
}

void training(network* network, training_image input, long epoch, double learning_rate){

    for (long i = 0; i < epoch; i++) {

        for (size_t j = 0; j < input.nb_images; j++) {

            //Forward
            layer* last_layer = forwardpass(network, input.images[j]);
            int res = getmax(last_layer);

            //Calc array in tmp format
            double expected[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            expected[(int)input.labels[j]] = 1;

            printf("Enter: %d      Get: %d      Similarity: %f %% \n",input.labels[j], res, cross_entropy(last_layer,expected));

            //Backward
            back_prop(network, input.images[j], expected, learning_rate);
        }
    }
}

neuron* neuron_init(long nb_weight, long previous_layer_size){
    neuron* a = malloc(sizeof(neuron));
    a->value = 0;
    a->bias = randfrom(0, sqrt( 2 / (double) previous_layer_size));
    a->weight_length = nb_weight;
    a->weight = calloc(nb_weight,sizeof(double));
    for(int k = 0; k < nb_weight; k++) {
        a->weight[k] = randfrom(0, sqrt( 2 / (double)previous_layer_size));
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


int neural_network(double* input, char train){

    network* a = NULL;
    if(!access( "save/Network.txt", F_OK))
        a = LoadNetwork();
    else
        a = initialisation();

    if(train) {
        training_image *pTrainingImage = SetupTrainingArrays();
        training(a, *pTrainingImage, 100, 0.01f);
        SaveNetwork(a);
        FreeTrainingArrays(pTrainingImage);
        free_network(a);
    }
    else{
        int res = getmax(forwardpass(a, input));
        free_network(a);
        return res;
    }

    return 0;
}