#include "solver.h"
#include "saver.h"
#include <stdio.h>
#include <stdlib.h>

const char *ext = "_solved";
const int lenext = 7;
const char *savefolder = "solved/";
const int lensavefolder = 7;

void help_text()
{
    printf("Use formats from the following :\n");
    printf("./main load_path save_path => ");
    printf("loads sudoku from file at load_path, solves it, ");
    printf("and it it is solved saves it at save_path.\n\n");

    printf("./main load_path => loads sudoku from file at load_path, ");
    printf("solves it and if it is solved saves it to the same path, ");
    printf("with '%s' added before the extension.\n",ext);
}

void solved_text(int res)
{
    if(res)
        printf("Sudoku is solved !\n");
    else
        printf("Sudoku can't be solved.\n");
}

int _strcmp(char *str1, const char *str2)
{
    while(*str1 != '\0' && *str2 != '\0' && *str1 == *str2)
    {
        str1++;
        str2++;
    }

    return *str1 == '\0' && *str2 == '\0';
}

void insert_str(char *str1,const char *str2,char *dst,unsigned int n)
{
    unsigned int i = 0;
    while(i < n)
    {
        *dst = *str1;
        dst++;
        str1++;
        i++;
    }
    while(*str2 != '\0')
    {
        *dst = *str2;
        dst++;
        str2++;
    }
    while(*str1 != '\0')
    {
        *dst = *str1;
        dst++;
        str1++;
    }
    *dst = '\0';
}

void add_solve_to_path(char *path,char *newpath)
{
    char *savepath = path;
    int lenpath = 0;
    int idot = 0;
    while(*path != '\0')
    {
        if(*path == '.')
            idot = lenpath;
        path++;
        lenpath++;
    }
    if(idot == 0)
        idot = lenpath;
    insert_str(savepath,"_solved",newpath,idot);
}


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


int _abort()
{
    printf("Aborting operation.\n");
    return 1;
}

int loadsolvesave(char *loadpath,char *savepath)
{
    printf("Loading grid from '%s'.\n",loadpath);
    char grid[81];
    if(!loadSudoku(loadpath,grid))
    {
        printf("\nOriginal sudoku grid:\n\n");
        print_sudoku(grid);
        //printf("\n");
        int solved = solveSudoku(grid);
        solved_text(solved);
        if(solved)
        {
            printf("\nSolved sudoku grid:\n\n");
            print_sudoku(grid);
            //printf("\n");
        }

        printf("Saving result grid to '%s'.\n",savepath);
        if(saveSudoku(grid,savepath))
            return _abort();
    }
    else
        return _abort();
    return 0;
}

int loadsolvesave_samepath(char *path)
{
    int lenpath = 0;
    char *loadpath = path;
    while(*path != '\0')
    {
        lenpath++;
        path++;
    }

    char *savepath = malloc((lenpath + lenext) * sizeof(char));
    add_solve_to_path(loadpath,savepath);
    int res = loadsolvesave(loadpath,savepath);
    free(savepath);
    return res;
}


int main(int argc, char** argv)
{
    int res = 0;
    if(argc == 1)
    {
        printf("Error : No arguments !\n");
        printf("Use argument -h for help.\n");
        res = 1;
    }
    else if(_strcmp(argv[1],"-h"))
    {
        help_text();
    }
    else if(argc == 2)
    {
        res = loadsolvesave_samepath(argv[1]);
    }
    else if(argc == 3)
    {
        res = loadsolvesave(argv[1],argv[2]);
    }

    return res;
}

