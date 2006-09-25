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
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <complearn/complearn.h>

struct TagManager {
  struct DataBlock *db;
  unsigned char *cur;
  int read;
  int size;
};

struct TagManager *clNewTagManager(struct DataBlock *db)
{
  struct TagManager *tm = clCalloc(sizeof(*tm),1);
  struct TagHdr *h = (struct TagHdr *) clDatablockData(db);
  tm->db = db;
  tm->cur = clDatablockData(db) + sizeof(struct TagHdr);
  tm->size = h->size;
  tm->read = 0;
  return tm;
}

t_tagtype clGetCurTagNum(const struct TagManager *tm)
{
  struct TagHdr *h = (struct TagHdr *) tm->cur;
  return h->tagnum;
}

void clStepNextDataBlock(struct TagManager *tm)
{
  struct TagHdr *h = (struct TagHdr *) tm->cur;
  tm->cur += h->size + sizeof(*h);
  tm->read += h->size + sizeof(*h);
}

struct DataBlock *clGetCurDataBlock(struct TagManager *tm)
{
  struct TagHdr *h = (struct TagHdr *) tm->cur;
  if (tm->read >= tm->size) return NULL;
  return clDatablockNewFromBlock(tm->cur, h->size + sizeof(*h));
}

void clFreeTagManager(struct TagManager *tm)
{
  tm->cur = NULL;
  tm->db = NULL;
  clFreeandclear(tm);
}

struct DataBlock *clPackageDataBlocks(t_tagtype overalltag, ...)
{
  va_list ap;
  struct DataBlock *db, *result;
  struct DRA *parts = clDraNew();
  va_start(ap, overalltag);
  while ( (db = va_arg(ap, struct DataBlock *)) ) {
    union PCTypes p = zeropct;
    p.dbp = db;
    clDraPush(parts, p);
  }
  va_end(ap);

  result = clPackageDdDataBlocks(overalltag,parts);

  clDraFree(parts);
  return result;
}

struct DataBlock *clPackageDdDataBlocks(t_tagtype tnum, struct DRA *parts)
{
  struct DataBlock *result;
  struct TagHdr h;
  int i, ressize;
  unsigned char *ptr;
  int dbsize = 0;
  unsigned char *dbptr, *resptr;

  for ( i = 0; i < clDraSize(parts); i += 1) {
    dbsize += clDatablockSize(clDraGetValueAt(parts,i).dbp);
  }
  dbptr = clCalloc(dbsize,1);
  ptr = dbptr;

  for ( i = 0; i < clDraSize(parts); i += 1) {
    memcpy(ptr, clDatablockData(clDraGetValueAt(parts,i).dbp), clDatablockSize(clDraGetValueAt(parts,i).dbp));
    ptr += clDatablockSize(clDraGetValueAt(parts,i).dbp);
  }

  h.tagnum = tnum;
  h.size = dbsize;

  ressize = sizeof(h) + dbsize;
  resptr = clCalloc(ressize,1);
  memcpy(resptr, &h, sizeof(h));
  memcpy(resptr + sizeof(h), dbptr, h.size);

  clFree(dbptr);

  result =  clDatablockNewFromBlock(resptr,ressize);
  clFree(resptr);
  return result;

}

void clFreeDataBlockpackage ( struct DRA *da )
{
  int i;
  for ( i = 0; i < clDraSize(da) ; i += 1) {
    clDatablockFreePtr(clDraGetValueAt(da,i).idbp.db);
  }
  clDraFree(da);
}

struct DRA *clLoadDatablockPackage(struct DataBlock *db)
{
  struct DRA *result = clDraNew();
  struct TagManager *tm;
  struct DataBlock *cur = NULL;

  tm = clNewTagManager(db);

  while ((cur = clGetCurDataBlock(tm))) {
    union PCTypes p = zeropct;
    p.idbp.tnum = clGetCurTagNum(tm);
    p.idbp.db = clDatablockClonePtr(cur);
    clDraPush(result, p);
    clStepNextDataBlock(tm);
    clDatablockFreePtr(cur);
  }
  clFreeTagManager(tm);

  return result;
}

struct DataBlock *clScanForTag(struct DRA *dd, int tnum)
{
  int i;
  struct DataBlock *db;
  t_tagtype curtnum;
  for (i = 0; i < clDraSize(dd); i += 1) {
    curtnum = clDraGetValueAt(dd,i).idbp.tnum;
    if (curtnum == tnum) {
      db = clDraGetValueAt(dd,i).idbp.db;
      return clDatablockClonePtr(db);
    }
  }
  return NULL;
}
