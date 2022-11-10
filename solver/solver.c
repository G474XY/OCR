#include <stdio.h>
#include <string.h>
#include "saver.h"

void clear_array(char grid[],int length)
{
    for(int i = 0; i < length; i++)
        grid[i] = 0;
}

void get_if_in_grid(char grid[],int y,int x, char res[])
{
    int i = y * 9 + x;
//    printf("Analyzing %d\n",grid[i]);
    if(grid[i] >= 1)
    {
        res[grid[i]-1] = 1;
//        printf("found number: %d\n",grid[i]);
    }
}


void get_valid_numbers(char grid[], int y, int x,char res[])
{
    clear_array(res,9);
    for(int line = 0; line < 9; line++)
    {
        get_if_in_grid(grid,line,x,res);
    }
    for(int col = 0; col < 9; col++)
    {
        get_if_in_grid(grid,y,col,res);
    }
    int x3 = (x / 3) * 3;
    int y3 = (y / 3) * 3;
    for(int yg = 0; yg < 3; yg++)
    {
        for(int xg = 0; xg < 3; xg++)
        {
            get_if_in_grid(grid,y3+yg,x3+xg,res);
        }
    }
}

int solve_rec(char grid[],int y, int x,int d)
{
    if(y >= 9)
        return 1;
    int i = y * 9 + x;
    int xn = (x + 1) % 9;
    int yn = y + (xn == 0);

    if(grid[i] != 0)
        return solve_rec(grid,yn,xn,d+1);
    char toCheck[9];
    get_valid_numbers(grid,y,x,toCheck);
    char num = 1;
    while(num <= 9)
    {
        if(toCheck[num-1] == 0) //Inverted, I know
        {
            grid[i] = num;
            if(solve_rec(grid,yn,xn,d+1))
                return 1;
            grid[i] = 0;
        }
        num++;
    }
    return 0;
}

int solveSudoku(char grid[])
{
    return solve_rec(grid,0,0,0);
}


int loadSudoku(char* path,char grid[])
{
    return load_sudoku(path,grid);
}

int saveSudoku(char grid[], char* path)
{
    return save_sudoku(grid,path);
}


