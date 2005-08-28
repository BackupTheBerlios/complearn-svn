#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <complearn/complearn.h>

#if NOGALLOC

/* Macros are used in the .h instead of functions with NOGALLOC enabled */

#else

char *clStrdup(const char *inp)
{
  char *n = clMalloc(strlen(inp) + 1);
  memcpy(n, inp, strlen(inp) + 1);
//  printf("%p: %s strdup\n", n, n);
  return n;
}

void *clMalloc(size_t size)
{
  void *ptr;
  int whoami;
  assert(size >= 0);
  assert(size < 10000000);
  ptr = malloc(size);
  whoami = getuid();
  if ( ( (whoami == 1000) &&
 /* 0xb7193808|s3' (1032 bytes) */
           (size == 44 && ptr == (void *) 0x4747342) ) ||

       ( (whoami == 1001) &&

           (size == 651 && ptr == (unsigned char *) 0xb7b0e408)

       )
     )  /* lisp is just alright */ {
    printf("%p: Warning, dangerous size %d allocated\n", ptr, size);
  }
  return ptr;
}

void *clCalloc(size_t nmem, size_t size)
{
  void *ptr;
  if (nmem == 0 || size == 0) {
    printf("Bad clCalloc request: %d, %d\n", nmem, size);
  }
  assert(nmem > 0 && size > 0);
  ptr =  clMalloc(nmem * size);
  assert(ptr);
  memset(ptr, 0, nmem * size);
  return ptr;
}

void clFree(void *ptr)
{
  assert(ptr);
  free(ptr);
  ptr = NULL;
}

#endif
