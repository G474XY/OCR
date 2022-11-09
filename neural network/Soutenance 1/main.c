#include "XorNN.h"


int main(int argc, char **argv) {
    if(argc < 2 || argc > 3){
        printf("Erreur : mauvais nombre de parametre\n");
        return 1;
    }
    if(argc == 2 && strcmp(argv[1], "-t") != 0){
        printf("Erreur : mode inexistant\n");
        return 2;
    }
    if(argc == 3){
        if((strcmp(argv[1], "0") != 0 && strcmp(argv[1], "1") != 0) || (strcmp(argv[2], "0") != 0 && strcmp(argv[2], "1") != 0)){
            printf("Erreur : mauvaises entrées\n");
            return 3;
        }
    }
    xor(argv);

    return 0;
}
