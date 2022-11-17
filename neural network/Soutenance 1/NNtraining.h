#ifndef NNTRAINING
#define NNTRAINING

void print_img(double** arr); //Affiche une image en 'ascii art'.
double** GetNextImage(); //Renvoie la prochaine image. Même array, initialisé.
void EndTraining(); //Optionnelle. Finit proprement l'entraînement.

#endif