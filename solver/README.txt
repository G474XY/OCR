Fonctions du solver toutes présentes dans le fichier 'solver.h'.

- int loadSudoku(char* path,char path[])
-> Charge le sudoku au chemin 'path' dans la grille 'grid' en place.
   Renvoie 1 si une erreur, 0 sinon.

- int saveSudoku(char grid[], char* path)
-> Sauvegarde le sudoku 'grid' dans le fichier 'path'.
   Aucune erreur n'est censée être renvoyée (toujours 0).

- int solveSudoku(char grid[])
-> Tente de résoudre le sudoku 'grid'.
   Renvoie 1 si réussi (+ grille résolue), sinon renvoie 0 et grille intacte.
