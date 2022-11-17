#include <stdio.h>
#include <stdlib.h>
#include "allocfree.h"

//==========CONSTANTS==========

const char* training_path = "training/testing.idx3-ubyte";
const char* labels_path = "training/testing_labels.idx1-ubyte";
const int img_size = 28;
const size_t uns_char_size = sizeof(unsigned char);
//=============================

//=======GLOBAL VARIABLES======
FILE* file = NULL;
FILE* l_file = NULL;
unsigned char buff[6];
unsigned char** img = NULL;
int num_images = 0;
//=============================

//========TEST FUNCTIONS=======
void print_img(unsigned char** arr)
{
    unsigned char b = 0;
    for(int i = 0; i < img_size; i++)
    {
        printf("[ ");
        for(int j = 0; j < img_size; j++)
        {
            b = ' ';
            if(arr[i][j] != 0)
                b = '0';
            printf("%c ",b);
        }
        printf("]\n");
    }
}
//=============================

//===========HELPERS===========
/*
Récupère un uint32 (unsigned long) depuis les 4 premiers bytes du buffer.
Bit de poids fort en premier.
*/
unsigned long _uint32_from_buff()
{
    unsigned long var = 0;
    var |= buff[0] << 24;
    var |= buff[1] << 16;
    var |= buff[2] << 8;
    var |= buff[3];
    return var;
}

/*
Lit une quantité de bytes dans le buffer.
Renvoie 1 si il y eut une erreur, 0 sinon.
*/
char read_bytes(FILE* f, size_t bytes)
{
    if(fread(buff,uns_char_size,bytes,f) < bytes)
    {
        printf("Error : read less bytes than expected.\n");
        return 1;
    }
    return 0;
}

/*
Lit un Uint32 et le stocke 'dans' le pointeur donné.
Renvoie 1 si il y eut une erreur, 0 sinon.
*/
char read_uint32(FILE* f,unsigned long* ptr)
{
    char err = read_bytes(f,4);
    *ptr = _uint32_from_buff();
    return err;
}

/*
Lit un simple byte et le stocke 'dans' le pointeur donné.
Renvoie 1 si il y eut une erreur, 0 sinon.
*/
char read_ubyte(FILE* f, unsigned char* ptr)
{
    char err = read_bytes(f,1);
    *ptr = buff[0];
    return err;
}
//=============================

//=========SUBFUNCTIONS========
/*
Initie la lecture du fichier pour les images.
Renvoie 1 si il y eut une erreur de path, 0 sinon.
*/
char file_init()
{
    file = fopen(training_path,"rb"); 
    l_file = fopen(labels_path,"rb");
    if(file == NULL)
    {
        printf("Error : training path doesn't exist\n");
        return 1;
    }
    if(l_file == NULL)
    {
        printf("Error : label path doesn't exist\n");
        return 1;
    }
    unsigned long tmp = 0;
    unsigned long tmp2 = 0;
    read_uint32(file,&tmp); //Magic Number
    read_uint32(file,&tmp2); //Nombre d'images
    read_uint32(file,&tmp); //Nombre de lignes
    read_uint32(file,&tmp); //Nombre de colonnes

    read_uint32(l_file,&tmp); //Magic Number
    read_uint32(l_file,&tmp); //Nombre de labels

    if(tmp2 != tmp)
    {
        printf("Error : reading files with different amounts of data\n");
        return 1;
    }

    return 0;
}

/*
Termine la lecture des fichiers et free les arrays utilisés.
*/
void file_end()
{
    fclose(file);
    fclose(l_file);
    _freeArrayArray(img,img_size);
}

/*
Récupère la prochaine image du fichier et la stocke dans
l'array donné en paramètre.
Renvoie 1 si il y eut une erreur, 0 sinon.
*/
char file_image(unsigned char** arr)
{
    unsigned char b = 0;
    for(int i = 0; i < img_size; i++)
    {
        for(int j = 0; j < img_size; j++)
        {
            if(read_ubyte(file,&b))
                return 1;
            arr[i][j] = (b > 0);
        }
    }
    return 0;
}

/*
Récupère le label de la prochaine image dans le fichier correspondant.
*/
char file_label(unsigned char* label)
{
    //Lit le prochain byte et le stocke 'dans' label.
    return read_ubyte(l_file,label);
}
//=============================

//========MAIN FUNCTION========
/*
Fonction à appeler par le réseau de neurones.
Récupère la prochaine image d'entraînement dans le fichier
de données ; ouvre celui-ci si première image demandée.

Son unique paramètre est un pointeur vers un unsigned char
représentant le nombre associé à l'image demandée.

Retourne l'array à deux dimension représentant l'image de
28*28 caractères.
0 représente un pixel vide
1 représente un pixel plein.
Remarque : d'un appel à l'autre, le pointeur ne change pas :
le tableau n'est alloué qu'une seule fois.
*/
unsigned char** GetNextImage(unsigned char* label)
{
    if(file == NULL)
    {
        //Premier appel : il faut initialiser les choses.
        file_init();
        img = _allocArrayArray(img_size,img_size);
    }

    //On lit la prochaine image du fichier
    if(file_image(img) || file_label(label))
    {
        //Si il y a une erreur (incluant la fin de parcours)
        file_end(); //On clôture le parcours (et on free l'image)
        return NULL; //On fait comprendre au réseau
        //qu'il faut arrêter d'appeler des images.
    }

    return img; //L'image obtenue est renvoyée.
}

/*
Termine la lecture des fichiers et free l'image utilisée.
*/
void EndTraining()
{
    file_end();
}
//=============================

//==========TEST MAIN==========
/*int main()
{
    unsigned char label = 0;
    size_t i = 0;
    while(!(img == NULL && i > 0))
    {
        GetNextImage(&label);
        i++;
    }
    return 0;
}*/
//=============================