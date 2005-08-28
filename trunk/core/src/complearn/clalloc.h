#ifndef __GALLOC_H
#define __GALLOC_H

#include <stdlib.h>

/*! \file clalloc.h */

#if NOGALLOC
#include <dmalloc.h>

#define clMalloc(x) malloc(x)
#define clCalloc(x,y) calloc(x,y)
#define clFree(x) free(x)
#define clStrdup(x) strdup(x)

#else

void *clMalloc(size_t size);
void *clCalloc(size_t nmem, size_t size);
void clFree(void *ptr);
char *clStrdup(const char *inp);

#endif

#define clFreeandclear(x) do { clFree(x); x = NULL; } while(0)
#define clFreeifpresent(x) do { if (x) clFreeandclear(x); } while (0)

#endif
