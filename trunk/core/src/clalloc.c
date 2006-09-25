/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
    printf("%p: Warning, dangerous size %d allocated\n", ptr, (int) size);
  }
  return ptr;
}

void *clCalloc(size_t nmem, size_t size)
{
  void *ptr;
  if (nmem == 0 || size == 0) {
    printf("Bad clCalloc request: %d, %d\n", (int) nmem, (int) size);
  }
  assert(nmem > 0 && size > 0);
  ptr =  clMalloc(nmem * size);
  assert(ptr);
  memset(ptr, 0, nmem * size);
  return ptr;
}

void *clRealloc(void *ptr, size_t size)
{
  void *result = realloc(ptr,size);
  if (result == NULL)
    clLogError("realloc error\n");
  return result;
}

void clFree(void *ptr)
{
  assert(ptr);
  if (ptr == NULL)
    clLogError("tried to free NULL pointer\n");
  free(ptr);
  ptr = NULL;
}

#endif
