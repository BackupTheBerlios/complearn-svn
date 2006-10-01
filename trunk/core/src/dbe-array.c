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
#include <string.h>
#include <stdio.h>


#include <complearn/complearn.h>

/** \brief Private enumeration instance for an array enumeration.
 *
 *  \struct DBEArrayEnumeration
 */
struct DBEArrayEnumeration
{
  struct DataBlock **db;
  int size;
};

/** \brief Iterator for a DBEArrayEnumeration.
 *
 *  \struct DBEArrayEnumerationIterator
 */
struct DBEArrayEnumerationIterator
{
  int cur;
  char curlabel[32];
};

static struct DataBlockEnumerationIterator *dbe_ar_newenumiter(struct DataBlockEnumeration *ptr)
{
  return clCalloc(sizeof(struct DBEArrayEnumerationIterator), 1);
}

static void dbe_ar_iterfree(struct DataBlockEnumerationIterator *dbi)
{
  clFreeandclear(dbi);
}
static void dbe_ar_enumfree(struct DataBlockEnumeration *dbe)
{
  struct DBEArrayEnumeration *ardbe = (struct DBEArrayEnumeration *) dbe->eptr;
  clFreeandclear(ardbe->db);
  clFreeandclear(dbe->eptr);
  clFreeandclear(dbe);
}
static struct DataBlock *dbe_ar_istar(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEArrayEnumeration *ardbe = (struct DBEArrayEnumeration *) dbe->eptr;
  struct DBEArrayEnumerationIterator *ardbi = (struct DBEArrayEnumerationIterator *) dbi;
  struct DataBlock **old, *cur = NULL;
  old = (ardbi->cur >= 0 && ardbi->cur < ardbe->size) ? ardbe->db + ardbi->cur : NULL;
  if (old)
    cur = clDatablockClonePtr(*old);
  return cur;
}
static char *dbe_ar_ilabel(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEArrayEnumerationIterator *ardbi = (struct DBEArrayEnumerationIterator *) dbi;
  sprintf(ardbi->curlabel, "ar-%d", ardbi->cur);
  return ardbi->curlabel;
}

static void dbe_ar_istep(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEArrayEnumeration *ardbe = (struct DBEArrayEnumeration *) dbe->eptr;
  struct DBEArrayEnumerationIterator *ardbi = (struct DBEArrayEnumerationIterator *) dbi;
  if (ardbi->cur < ardbe->size)
    ardbi->cur += 1;
}

struct DataBlockEnumeration *clBlockEnumerationLoadArray(struct DataBlock **db, int size)
{
  struct DataBlockEnumeration e = {
    NULL, /* eptr, private enumeration instance */
    dbe_ar_newenumiter,
    dbe_ar_istep,
    dbe_ar_iterfree,
    dbe_ar_enumfree,
    dbe_ar_istar,
    dbe_ar_ilabel,
  };
  struct DataBlockEnumeration *dbe;
  struct DBEArrayEnumeration *ardbe;
  assert(size > 0);
  dbe = clCalloc(sizeof(struct DataBlockEnumeration),1);
  *dbe = e;
  dbe->eptr = clCalloc(sizeof(struct DBEArrayEnumeration), 1);
  ardbe = (struct DBEArrayEnumeration *) dbe->eptr;
  ardbe->db = clCalloc(sizeof(struct DataBlock *), size);
  memcpy(ardbe->db, db, sizeof(struct DataBlock *) * size);
  ardbe->size = size;
  return dbe;
}
