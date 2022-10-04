#include <stdio.h>
#include <string.h>
#include "saver.h"








void clear_array(char grid[],int length)
{
    for(int i = 0; i < length; i++)
        grid[i] = 0;
}

void print_array(char a[],int length)
{
    printf("[");
    for(int i = 0; i < length; i++)
    {
        printf("%d, ",a[i]);
    }
    printf("]\n");
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
    /*if((x%3 == 1) && (y %)
        get_if_in_grid(grid,(y-1)*9+x-1,res);
    else if(((y-1)%3) && ((x+1)%3))
        get_if_in_grid(grid,(y-1)*9+x+1,res);
    else if(((y+1)%3) && ((x+1)%3))
        get_if_in_grid(grid,(y+1)*9+x+1,res);
    else if(((y+1)%3) && ((x-1)%3))
        get_if_in_grid(grid,(y+1)*9+x-1,res);*/
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
    //print_sudoku(grid);
    if(y >= 9)
        return 1;
    //printf("(%d,%d), depth = %d\n",y,x,d);
    int i = y * 9 + x;
    int xn = (x + 1) % 9;
    int yn = y + (xn == 0);

    if(grid[i] != 0)
        return solve_rec(grid,yn,xn,d+1);
    char toCheck[9];
    get_valid_numbers(grid,y,x,toCheck);
    //print_array(toCheck,9);
    char num = 1;
    while(num <= 9)
    {
        if(toCheck[num-1] == 0) //Inverted, I know
        {
            //printf("((%d, %d) ; depth %d) : trying %d\n",y,x,d,num);
            grid[i] = num;
            if(solve_rec(grid,yn,xn,d+1))
                return 1;
            grid[i] = 0;
            //printf("((%d, %d) ; depth %d) : %d didn't work\n",y,x,d,num);
        }
        num++;
    }
    return 0;
}

int solve(char grid[])
{
    return solve_rec(grid,0,0,0);
}

