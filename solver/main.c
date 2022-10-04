#include "solver.h"
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
    printf("%d\n",solve(grid));
}

int main(int argc, char** argv)
{
    if(argc == 2 && !strcmp(argv[1],"solve"))
        solve_test(s1);

    return 0;
}

