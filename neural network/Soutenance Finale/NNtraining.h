#ifndef NNTRAINING
#define NNTRAINING

typedef struct training
{
    size_t nb_images; //Le nbre d'images contenus dans l'array.
    int im_size; //Taille de l'image (carrée).
    double** images; //Array des images de 28 * 28.
    char* labels; //Array des labels associés à chacune des images.
} training;

void print_img(double* arr); //Affiche une image en 'ascii art'.

/*
Libère le struct de données ainsi que tout ce qu'il contient.
*/
void FreeTrainingArrays(training* training);

/*
Renvoie un struct qui contient les infos pour le training
avec la base de données du MIDST.
*/
training* SetupTrainingArrays();

#endif