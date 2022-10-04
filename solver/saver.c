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
            /*if(!(gi % 3) && x < 8)
            {
                str[si] = ' ';
                si++;
            }*/
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

int load_sudoku(char* path,char grid[])
{
    FILE* file = NULL;
    file = fopen(path,"r");
    if(file == NULL)
    {
        printf("Error : path is null : %s\n",path);
        return 1;
    }
    int i = 0;
    char c;
    while(!feof(file))
    {
        c = fgetc(file) - '0';
        if(c < 0 || c > 9)
            continue;
        if(i >= 81)
        {
            printf("Error : bad sudoku format(too many numbers)(%d)\n",c);
            return 1;
        }
        grid[i] = c;
        i++;
    }
    if(i < 81)
    {
        printf("Error : bad sudoku format(not enough numbers)\n");
        return 1;
    }
    fclose(file);
    return 0;
}


int save_sudoku(char grid[], char* path)
{
    char str[512];
    sudoku_to_str(grid,str);
    FILE* file;
    file = fopen(path,"w");
    fprintf(file,"%s",str);
    fclose(file);
    return 0;
}
