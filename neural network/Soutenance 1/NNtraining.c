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
unsigned long _uint32_from_buff()
{
    unsigned long var = 0;
    var |= buff[0] << 24;
    var |= buff[1] << 16;
    var |= buff[2] << 8;
    var |= buff[3];
    return var;
}

char read_bytes(size_t bytes)
{
    /*
    Lit une quantité de bytes dans le buffer.
    Renvoie 1 si il y eut une erreur, 0 sinon.
    */
    if(fread(buff,uns_char_size,bytes,file) < bytes)
    {
        printf("Error : read less bytes than expected.\n");
        return 1;
    }
    return 0;
}

char read_uint32(unsigned long* ptr)
{
    /*
    Lit un Uint32 et le stocke 'dans' le pointeur donné.
    Renvoie 1 si il y eut une erreur, 0 sinon.
    */
    char err = read_bytes(4);
    *ptr = _uint32_from_buff();
    return err;
}

unsigned char read_ubyte(unsigned char* ptr)
{
    /*
    Lit un simple byte et le stocke 'dans' le pointeur donné.
    Renvoie 1 si il y eut une erreur, 0 sinon.
    */
    char err = read_bytes(1);
    *ptr = buff[0];
    return err;
}
//=============================

//=========SUBFUNCTIONS========
char file_init()
{
    /*
    Initie la lecture du fichier pour les images.
    Renvoie 1 si il y eut une erreur de path, 0 sinon.
    */
    file = fopen(training_path,"rb"); 
    //On initie le double array qu'est l'image
    if(file == NULL)
    {
        printf("Error : training path doesn't exist\n");
        return 1;
    }
    unsigned long tmp = 0;
    read_uint32(&tmp); //Magic Number
    read_uint32(&tmp); //Nombre d'images
    read_uint32(&tmp); //Nombre de lignes
    read_uint32(&tmp); //Nombre de colonnes

    return 0;
}

void file_end()
{
    _freeArrayArray(img,img_size);
}

char file_image(unsigned char** arr)
{
    /*
    Récupère la prochaine image du fichier et la stocke dans
    l'array donné en paramètre.
    Renvoie 1 si il y eut une erreur, 0 sinon.
    */
    unsigned char b = 0;
    char err = 0;
    for(int i = 0; i < img_size; i++)
    {
        for(int j = 0; j < img_size; j++)
        {
            err = read_ubyte(&b);
            if(err)
                return 1;
            arr[i][j] = (b > 0);
        }
    }
    return 0;
}
//=============================

//========MAIN FUNCTION========
unsigned char** GetNextImage()
{
    /*
    Fonction à appeler par le réseau de neurones.
    Récupère la prochaine image d'entraînement dans le fichier
    de données ; ouvre celui-ci si première image demandée.
    
    Retourne l'array à deux dimension représentant l'image de
    28*28 caractères.
    0 représente un pixel vide
    1 représente un pixel plein.
    */
    
    if(file == NULL)
    {
        //Premier appel : il faut initialiser les choses.
        file_init();
        img = _allocArrayArray(img_size,img_size);
    }

    //On lit la prochaine image du fichier
    if(file_image(img))
    {
        //Si il y a une erreur (incluant la fin de parcours)
        file_end(); //On clôture le parcours (et on free l'image)
        return NULL; //On fait comprendre au réseau
        //qu'il faut arrêter d'appeler des images.
    }
    /*if(i < 100)
        print_img(img);*/

    return img; //L'image obtenue est renvoyée.
}
//=============================

//==========TEST MAIN==========
/*int main()
{
    for(int i = 0; i < 10000; i++)
    {
        GetNextImage(i);
    }
    return 0;
}*/
//=============================