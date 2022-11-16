#include <stdlib.h>

unsigned char* _allocArray(size_t size)
{
    unsigned char* res = malloc(size * sizeof(unsigned char));
    return res;
}

unsigned char** _allocArrayArray(size_t size1,size_t size2)
{
    unsigned char** res = malloc(size1 * sizeof(unsigned char*));
    for(size_t i = 0; i < size1; i++)
    {
        res[i] = _allocArray(size2);
    }
    return res;
}

void _freeArrayArray(unsigned char** arr,size_t size1)
{
    for(size_t i = 0; i < size1; i++)
    {
        free(arr[i]);
    }
    free(arr);
}