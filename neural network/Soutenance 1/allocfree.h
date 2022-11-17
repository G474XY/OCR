#ifndef ALLOCFREE
#define ALLOCFREE

#include <stdlib.h>

double* _allocArray(size_t size);
double** _allocArrayArray(size_t size1,size_t size2);
void _freeArrayArray(double** arr,size_t size1);

#endif