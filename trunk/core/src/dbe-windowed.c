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
#include <stdio.h>


#include <complearn/complearn.h>

/** \brief Private enumeration instance for windowed enumeration.
 *
 * This DBE enumerates through successive (possibly overlapping) windows
 * of regular stepsize, adjustable stride, beginpoint, and endpoint.
 *
 *  \struct DBEWindowedEnumeration
 */
struct DBEWindowedEnumeration
{
  struct DataBlock *db;
  int firstpos, stepsize, width, lastpos;
};

/** \brief Iterator for a DBEWindowedEnumeration.
 *
 * This iterator goes through successive windows of data.
 *
 *  \struct DBEWindowedEnumerationIterator
 */
struct DBEWindowedEnumerationIterator
{
  int curpos;
  unsigned char *dbptr;
  int dbsize;
  char curlabel[32];
};

static struct DataBlockEnumerationIterator *dbe_wi_newenumiter(struct DataBlockEnumeration *dbe)
{
  struct DBEWindowedEnumeration *widbe = (struct DBEWindowedEnumeration *) dbe->eptr;
  struct DBEWindowedEnumerationIterator *widbi;
  widbi = clCalloc(sizeof(struct DBEWindowedEnumerationIterator), 1);
  widbi->curpos = widbe->firstpos;
  return (struct DataBlockEnumerationIterator *) widbi;
}

static void dbe_wi_iterfree(struct DataBlockEnumerationIterator *dbi)
{
  struct DBEWindowedEnumerationIterator *widbi = (struct DBEWindowedEnumerationIterator *) dbi;
  widbi->dbptr = NULL;
  widbi->dbsize = 0;
  clFreeandclear(dbi);
}

static void dbe_wi_enumfree(struct DataBlockEnumeration *dbe)
{
  struct DBEWindowedEnumeration *widbe = (struct DBEWindowedEnumeration *) dbe->eptr;
  clDatablockFreePtr(widbe->db);
  widbe->db = NULL;
  clFreeandclear(dbe->eptr);
  clFreeandclear(dbe);
}
static struct DataBlock *dbe_wi_istar(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEWindowedEnumeration *widbe = (struct DBEWindowedEnumeration *) dbe->eptr;
  struct DBEWindowedEnumerationIterator *widbi = (struct DBEWindowedEnumerationIterator *) dbi;
  if (widbi->curpos >= 0 && widbi->curpos + widbe->width - 1 <= widbe->lastpos)
  {
   widbi->dbptr = clDatablockData(widbe->db) + widbi->curpos;
   widbi->dbsize = widbe->width;
   return clDatablockNewFromBlock(widbi->dbptr, widbi->dbsize);
  }
  else
    return NULL;
}

static char *dbe_wi_ilabel(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEWindowedEnumerationIterator *widbi = (struct DBEWindowedEnumerationIterator *) dbi;
  sprintf(widbi->curlabel, "wo-%d", widbi->curpos);
  return widbi->curlabel;
}

static void dbe_wi_istep(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEWindowedEnumeration *widbe = (struct DBEWindowedEnumeration *) dbe->eptr;
  struct DBEWindowedEnumerationIterator *widbi = (struct DBEWindowedEnumerationIterator *) dbi;
  if (widbi->curpos <= widbe->lastpos)
    widbi->curpos += widbe->stepsize;
}

struct DataBlockEnumeration *clBlockEnumerationLoadWindowed(struct DataBlock *db,
    int firstpos, int stepsize, int width, int lastpos)
{
  struct DataBlockEnumeration c = {
    NULL, /* eptr, private enumeration instance */
    dbe_wi_newenumiter,
    dbe_wi_istep,
    dbe_wi_iterfree,
    dbe_wi_enumfree,
    dbe_wi_istar,
    dbe_wi_ilabel,
  };
  struct DataBlockEnumeration *dbe;
  struct DBEWindowedEnumeration *widbe;
  clAssert(db != NULL);
  clAssert(clDatablockSize(db) > 0);
  clAssert(clDatablockSize(db) < 10000000); /* TODO: remove me */
  clAssert(stepsize > 0);
  clAssert(width > 0);
  clAssert(firstpos >= 0);
  clAssert(lastpos >= firstpos);
  dbe = clCalloc(sizeof(struct DataBlockEnumeration),1);
  *dbe = c;
  dbe->eptr = clCalloc(sizeof(struct DBEWindowedEnumeration), 1);
  widbe = (struct DBEWindowedEnumeration *) dbe->eptr;
  widbe->db = clDatablockClonePtr(db);
  widbe->firstpos = firstpos;
  widbe->stepsize = stepsize;
  widbe->width = width;
  widbe->lastpos = lastpos;
  return dbe;
}

