#include "solver.h"
#include "saver.h"
#include <stdio.h>
#include <string.h>

char s1[] = 
{
    1,2,7,0,0,4,5,8,0,
    5,0,0,7,2,1,0,0,3,
    4,0,3,0,0,0,0,0,0,
    2,1,0,0,6,7,0,0,4,
    0,7,0,0,0,0,2,0,0,
    6,3,0,0,4,9,0,0,1,
    3,0,6,0,0,0,0,0,0,
    0,0,0,1,5,8,0,0,6,
    0,0,0,0,0,6,9,5,0
};



void solve_test(char grid[])
{
    print_sudoku(grid);
    printf("%d\n",solve(grid));
    print_sudoku(grid);
}

void load(char* path)
{
    char grid[81];
    load_sudoku(path,grid);
    solve_test(grid);
}

void save(char grid[], char* path)
{
    solve_test(grid);
    save_sudoku(grid,path);
}

int main(int argc, char** argv)
{
    if(argc == 2 && !strcmp(argv[1],"solve"))
        solve_test(s1);
    if(argc == 3 && !strcmp(argv[1],"solvesave"))
        save(s1,argv[2]);
    if(argc == 3 && !strcmp(argv[1],"solveload"))
        load(argv[2]);
    return 0;
}

