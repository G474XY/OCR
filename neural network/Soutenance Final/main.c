#include "NN.h"


int main(int argc, char **argv) {
    if(argc < 2 || argc > 3){
        printf("Erreur : mauvais nombre de parametre\n");
        return 1;
    }
    if(argc == 2 && strcmp(argv[1], "-t") != 0){
        printf("Erreur : mode inexistant\n");
        return 2;
    }
    neural_network(NULL, 1);
    return 0;
}
