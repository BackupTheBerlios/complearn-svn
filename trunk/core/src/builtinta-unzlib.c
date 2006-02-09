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
#include <complearn/complearn.h>
#include <string.h>

#include <complearn/transadaptor.h>

static char *unzlib_shortname(void);
static void unzlib_transfree(struct TransformAdaptor *ta);
static int unzlib_predicate(struct DataBlock *db);
static struct DataBlock *unzlib_transform(struct DataBlock *src);

struct TransformAdaptor *clBuiltin_UNZLIB(void)
{
  struct ZlibDynamicAdaptor *zlib = clGrabZlibDA();
  struct TransformAdaptor *ptr;
	struct TransformAdaptor t =
	{
    sn:    unzlib_shortname,
    tfree: unzlib_transfree,
    pf:    unzlib_predicate,
    tf:    unzlib_transform,
    tptr:  NULL,
  };
  if (!zlib)
    return NULL;
  ptr = (struct TransformAdaptor*)clCalloc(sizeof(*ptr), 1);
  *ptr = t;
	return ptr;
}

static char *unzlib_shortname(void)
{
	return "unzlib";
}

static void unzlib_transfree(struct TransformAdaptor *ta)
{
  clFreeandclear(ta);
}

static int unzlib_predicate(struct DataBlock *db)
{
	return clDatablockSize(db) > 1 && clDatablockData(db)[0]==0x78 && clDatablockData(db)[1]==0xda;
}

static struct DataBlock *unzlib_transform(struct DataBlock *src)
{
  struct ZlibDynamicAdaptor *zlib = clGrabZlibDA();
	struct DataBlock *result;
  if (zlib) {
	int i;
	unsigned char *dbuff = NULL;
	int triedp;
	triedp = clDatablockSize(src) * 3.0 + 1;
	do {
    unsigned long p;
		if (dbuff != NULL)
			free(dbuff);
		dbuff = (unsigned char*)clMalloc(p);
    p = triedp;
		i = uncompress(dbuff, &p,clDatablockData(src),clDatablockSize(src));
		triedp = 2*triedp;
	} while (i == -5);  /* Z_BUF_ERROR */
  result = clDatablockNewFromBlock(dbuff, triedp);
	free(dbuff);
//	clDatablockFree(src); /* TODO: document this new non-free behavior */
   } else {
     assert ( 0 && "zlib not supported");
     exit(1);
   }
	return result;
}
