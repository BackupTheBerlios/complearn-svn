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
#include <string.h>
#include <complearn/complearn.h>

static char *unbz2a_shortname(void);
static void unbz2a_transfree(struct TransformAdaptor *ta);
static int unbz2a_predicate(struct DataBlock *db);
static struct DataBlock *unbz2a_transform(struct DataBlock *src);

struct TransformAdaptor *clBuiltin_UNBZIP(void)
{
  struct BZ2DynamicAdaptor *bzlib = clGrabBZ2DACB();
  struct TransformAdaptor *ptr;
  struct TransformAdaptor t =
  {
    sn:    unbz2a_shortname,
    tfree: unbz2a_transfree,
    pf:    unbz2a_predicate,
    tf:    unbz2a_transform,
    tptr:  NULL,
  };
  if (!bzlib)
    return NULL;
  ptr = (struct TransformAdaptor*)clCalloc(sizeof(*ptr), 1);
  *ptr = t;
  return ptr;
}

static char *unbz2a_shortname(void)
{
	return "unbzip";
}

static void unbz2a_transfree(struct TransformAdaptor *ta)
{
  clFreeandclear(ta);
}

static int unbz2a_predicate(struct DataBlock *db)
{
	return clDatablockSize(db) > 2 && clDatablockData(db)[0]==0x42 && clDatablockData(db)[1]==0x5a && clDatablockData(db)[2]==0x68;
}

static struct DataBlock *unbz2a_transform(struct DataBlock *src)
{
  struct BZ2DynamicAdaptor *bzlib = clGrabBZ2DACB();
  struct DataBlock *result = NULL;
  if (bzlib) {
  int i;
  unsigned char *dbuff = NULL;
  int p;
  p = clDatablockSize(src) * 3.0 + 1;
  do {
    if (dbuff != NULL)
      clFreeandclear(dbuff);
    dbuff = (unsigned char*)clMalloc(p);
    i = (bzlib->buftobufdecompress)((char *) dbuff,(unsigned int *) &p, (char *) clDatablockData(src),clDatablockSize(src), 0, 0);
    p = 2*p;
  } while (i != 0);
  result = clDatablockNewFromBlock(dbuff,p);
  clFreeandclear(dbuff);
  // clDatablockFree(src); /* TODO: document this */
   } else {
	assert ( 0 && "bzip not supported");
	exit(1);
   }
  return result;
}

