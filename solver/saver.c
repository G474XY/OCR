#include <stdio.h>



void sudoku_to_str(char grid[],char str[])
{
    int si = 0;
    int gi = 0;
    for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 9; x++)
        {
            if(grid[gi] > 0)
                str[si] = grid[gi] + '0';
            else
                str[si] = '.';
            si++;
            gi++;
            if(!(gi % 3) && x < 8)
            {
                str[si] = ' ';
                si++;
            }
        }
        str[si] = '\n';
        si++;
    }
    str[si] = '\0';
}

void print_sudoku(char grid[])
{
    int gi = 0;
    for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 9; x++)
        {
            if(grid[gi] > 0)
                printf("%d",grid[gi]);
            else
                printf(".");
            gi++;
            if(gi % 3 == 0 && (gi%9) < 8)
                printf(" ");
        }
        printf("\n");
        if(!((y+1)%3))
            printf("\n");
    }
}
