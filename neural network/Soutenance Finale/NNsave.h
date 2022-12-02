#include "NN.h"
#include <stdio.h>

#ifndef NN_SAVE
#define NN_SAVE

/*
Returns the loaded network from the save file.
NOTE : the save path is a constant defined in NNsave.c
(We may have to change that in the future.)
*/
network* load_network();

/*
Saves the network argument to the save file.
*/
void save_network(network* net);

#endif