#ifndef SAVER
#define SAVER

void sudoku_to_str(char grid[], char str[]);
void print_sudoku(char grid[]);
int load_sudoku(char* path, char grid[]);
int save_sudoku(char grid[],char* path);

#endif
