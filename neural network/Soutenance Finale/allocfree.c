#include <stdlib.h>

double* _allocArray(size_t size)
{
    double* res = malloc(size * sizeof(double));
    return res;
}

double** _allocArrayArray(size_t size1,size_t size2)
{
    double** res = malloc(size1 * sizeof(double*));
    for(size_t i = 0; i < size1; i++)
    {
        res[i] = _allocArray(size2);
    }
    return res;
}

void _freeArrayArray(double** arr,size_t size1)
{
    for(size_t i = 0; i < size1; i++)
    {
        free(arr[i]);
    }
    free(arr);
}