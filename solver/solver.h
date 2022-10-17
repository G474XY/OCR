#ifndef SOLVER
#define SOLVER

//Prend en entrée un sudoku : une grille de 9*9 de char.
//Le résoud en place si possible; renvoie le résultat (1 = réussi, 0 = raté)
int solveSudoku(char grid[]);

//Charge un sudoku dans une grille à partir d'un path.
//Renvoie 0 si pas d'erreur, 1 sinon (µ un petit print)
int loadSudoku(char* path,char grid[]);

//Sauvegarde une grille de sudoku dans un fichier path.
//(est censée) renvoyer toujours 0 : pas d'erreurs.
int saveSudoku(char grid[], char* path);

#endif
