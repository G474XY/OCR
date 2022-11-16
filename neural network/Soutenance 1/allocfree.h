#ifndef ALLOCFREE
#define ALLOCFREE

#include <stdlib.h>

unsigned char* _allocArray(size_t size);
unsigned char** _allocArrayArray(size_t size1,size_t size2);
void _freeArrayArray(unsigned char** arr,size_t size1);

#endif