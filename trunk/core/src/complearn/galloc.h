#ifndef __GALLOC_H
#define __GALLOC_H

#include <stdlib.h>

#if NOGALLOC
#include <dmalloc.h>

#define gmalloc(x) malloc(x)
#define gcalloc(x,y) calloc(x,y)
#define gfree(x) free(x)
#define gstrdup(x) strdup(x)

#else

void *gmalloc(size_t size);
void *gcalloc(size_t nmem, size_t size);
void gfree(void *ptr);
char *gstrdup(const char *inp);

#endif

#define gfreeandclear(x) do { gfree(x); x = NULL; } while(0)
#define gfreeifpresent(x) do { if (x) gfreeandclear(x); } while (0)

#endif
