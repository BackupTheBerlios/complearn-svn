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
#include <stdlib.h>
#include <string.h>
#include <complearn/dra.h>
#include <complearn/complearn.h>

const union PCTypes zeropct;
const union PCTypes onepcti = { .i = 1 };
static void makeSizeFor(struct DRA *da, int where);

struct DRA {
  int alloc, size, elemsize;
  t_doublefreeclFunc clFunc;
  void *udata;
  union PCTypes *pc;
};

/* The basic file header */

struct DRAHdr {
  int level;
  int size;
};

union PCTypes clDraRandom(const struct DRA *da)
{
  int i;
  if (da == NULL) {
    clLogError("NULL ptr in clDraRandom()\n");
  }
  assert(da->size > 0);
  i = rand() % da->size;
  return da->pc[i];
}

static void callUserFunc(struct DRA *ptr)
{
  if (!ptr->clFunc)
    return;
  ptr->clFunc(ptr, ptr->udata);
  ptr->clFunc = NULL;
  ptr->udata = NULL;
}

void clDraFree(struct DRA *ptr)
{
//  fprintf(stderr, "%p:%d ZFE DRA FREED\n", ptr, 0);
  assert(ptr);
  callUserFunc(ptr);
  assert(ptr->pc);
  clFreeandclear(ptr->pc);
  ptr->alloc = 0;
  ptr->size = 0;
  clFreeandclear(ptr);
}

struct DRA *clDraNew(void)
{
  static int counter=0;

  struct DRA *da = clCalloc(sizeof(struct DRA), 1);

  counter += 1;
  if (counter == -1) {
    printf("Counter is %d, exitting..\n", counter);
    exit(1);
  }
  da->alloc = 10;
  da->clFunc = NULL;
  da->elemsize = sizeof(union PCTypes);
  da->pc = clCalloc(da->elemsize, da->alloc);
//  fprintf(stderr, "%p:%d NEW DRA RETURNED\n", da, counter);
  return da;
}

int clDraSize(const struct DRA *a)
{
  if (a == NULL) {
    clLogError("NULL ptr in clDraSize()\n");
  }
  return a->size;
}

double clDraGetDValueAt(struct DRA *da, int where)
{
  if (da == NULL) {
    clLogError("NULL ptr in clDraGetDValueAt()\n");
  }
  return clDraGetValueAt(da, where).d;
}

void clDraVerify(const struct DRA *da)
{
  if (!(da) )
    goto isBad;
  if (!(da->size > 0) )
    goto isBad;
  if (!(da->pc) )
    goto isBad;
  if (!(da->alloc >= da->size) )
    goto isBad;
  return;
isBad:
  printf("Oh no, bad doubler!!\n");
  exit(1);
}

union PCTypes clDraGetValueAt(const struct DRA *da, int where)
{
#if LOGICWALL
  if (da == NULL) {
    clLogError("NULL ptr in clDraGetValueAt()\n");
  }
  assert(da);
  assert(da->pc && "tried to access freed DRA");
  clDraVerify(da);
  assert(where >= 0);
  assert(where < 1000000);  /* TODO: remove me */
  if (where >= da->size) {
    clLogError( "Error, trying to read element index %d but only of size %d\n",
        where, da->size);
    assert(where < da->size && "reading unitialized element in array");
  }
#endif
  return da->pc[where];
}

void clDraSetValueAt(struct DRA *da, int where, union PCTypes p)
{
  if (da == NULL) {
    clLogError("NULL ptr in clDraSetValueAt()\n");
  }
  assert(da);
  assert(where >= 0);
  makeSizeFor(da, where);
  da->pc[where] = p;
  if (where + 1 >= da->size)
    da->size = where + 1;
}

void clDraSetDValueAt(struct DRA *da, int where, double val)
{
  union PCTypes pc;
  if (da == NULL) {
    clLogError("NULL ptr in clDraSetDValueAt()\n");
  }
  pc.d = val;
  clDraSetValueAt(da, where, pc);
}

static void makeSizeFor(struct DRA *da, int where)
{
  if (da == NULL) {
    clLogError("NULL ptr in makeSizeFor()\n");
  }
  while (where >= da->alloc) {
    union PCTypes *newBlock;
    int newAlloc = da->alloc * 2;
    newBlock = clCalloc(da->elemsize, newAlloc);
    assert(newBlock);
    memcpy(newBlock, da->pc, da->elemsize * da->alloc);
    clFreeandclear(da->pc);
    da->pc = newBlock;
    da->alloc = newAlloc;
  }
}

union PCTypes clDraShift(struct DRA *da)
{
  union PCTypes result;
  if (da == NULL) {
    clLogError("NULL ptr in clDraShift()\n");
  }
  result = da->pc[0];
  assert(da->size > 0);
  memmove(da->pc, da->pc+1, (da->size-1) * sizeof(da->pc[0]));
  da->size -= 1;
  return result;
}

union PCTypes clDraPop(struct DRA *da)
{
  union PCTypes result;
  if (da == NULL) {
    clLogError("NULL ptr in clDraPop()\n");
  }
  result  = da->pc[da->size-1];
  assert(da->size > 0);
  memset(da->pc+(da->size-1), 0, sizeof(da->pc[0]));
  da->size -= 1;
  return result;
}

void clDraUnshift(struct DRA *da, union PCTypes p)
{
  if (da == NULL) {
    clLogError("NULL ptr in clDraUnshift()\n");
  }
  assert(da);
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->alloc >= 0);
  assert(da->alloc >= da->size);
  makeSizeFor(da, clDraSize(da));
  assert(da);
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->alloc >= 0);
  assert(da->alloc >= da->size);
  memmove(da->pc+1, da->pc, sizeof(da->pc[0]) * clDraSize(da));
  assert(da);
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->alloc >= 0);
  assert(da->alloc >= da->size);
  da->pc[0] = p;
  assert(da);
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->alloc >= 0);
  assert(da->alloc >= da->size);
  da->size += 1;
}

void clDraPush(struct DRA *da, union PCTypes p)
{
  if (da == NULL) {
    clLogError("NULL ptr in clDraPush()\n");
  }
#if LOGICWALL
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->size < 10000);
#endif
  clDraSetValueAt(da, da->size, p);
}

int clDraSwapAt(struct DRA *da, int inda, int indb)
{
  union PCTypes tmp;
  if (da == NULL) {
    clLogError("NULL ptr in clDraSwapAt()\n");
  }
  assert(da);
  assert(inda >= 0);
  assert(indb >= 0);
  assert(inda < da->size);
  assert(indb < da->size);
  tmp = da->pc[inda];
  da->pc[inda] = da->pc[indb];
  da->pc[indb] = tmp;
  return CL_OK;
}

struct DRA *clDraClone(const struct DRA *ptr)
{
  if (ptr == NULL) {
    clLogError("NULL ptr in clDraClone()\n");
  }
  assert(ptr);
  return clDraDeepClone(ptr, 0);
}

void clDraDeepFree(struct DRA *ptr, int lvl)
{
  if (ptr == NULL) {
    clLogError("NULL ptr in clDraDeepFree()\n");
  }
  assert(ptr);
  if (lvl) {
    int i;
    int sz = clDraSize(ptr);
    for (i = 0; i < sz; ++i)
      clDraDeepFree(clDraGetValueAt(ptr, i).ar, lvl-1);
  }
  clDraFree(ptr);
}

struct DRA *clDraDeepClone(const struct DRA *ptr, int lvl)
{
  struct DRA *result = clDraNew();
  int sz;
  int i;
  if (ptr == NULL) {
    clLogError("NULL ptr in clDraDeepClone()\n");
  }
  sz = clDraSize(ptr);
  for (i = 0; i < sz; ++i) {
    union PCTypes p = zeropct;
    if (lvl)
      p.ar = clDraDeepClone(clDraGetValueAt(ptr, i).ar, lvl-1);
    else
      p = clDraGetValueAt(ptr, i);
    clDraSetValueAt(result, i, p);
  }
  return result;
}

void clDraPrintIntList(const struct DRA *da)
{
  int i;
  if (da == NULL) {
    clLogError("NULL ptr in clDraPrintIntList()\n");
  }
  for (i = 0; i < da->size; ++i)
    printf("%d ", da->pc[i].i);
  printf("\n");
}

void clDraPrintIntPairList(const struct DRA *da)
{
  int i;
  if (da == NULL) {
    clLogError("NULL ptr in clDraPrintIntPairList()\n");
  }
  for (i = 0; i < da->size; i += 1)
    printf("(%d,%d) ", da->pc[i].ip.x, da->pc[i].ip.y);
  printf("\n");
}

struct DRA *clDraLoad(struct DataBlock *d, int fmustbe)
{
  int i;
  struct TagHdr *h;
  struct DRAHdr *ddh;
  struct DRA *result;
  unsigned char *cur;
  if (d == NULL) {
    clLogError("NULL ptr in clDraLoad()\n");
  }
  cur = clDatablockData(d) + sizeof(*h) + sizeof(*ddh);
  h = (struct TagHdr *) clDatablockData(d);
  ddh = (struct DRAHdr *) (clDatablockData(d) + sizeof(*h));
  if (h->tagnum != TAGNUM_DOUBLEDOUBLER) {
    clLogError("Error: expecting DOUBLEDOUBLER tagnum %x, got %x\n",
        TAGNUM_DOUBLEDOUBLER, h->tagnum);
    exit(1);
  }
  if (ddh->level) {
    result = clDraNew();
    for (i = 0; i < ddh->size; ++i) {
      union PCTypes p = zeropct;
      struct DataBlock *dbcur;
      dbcur = clDatablockNewFromBlock(cur,cur+clDatablockSize(d)-clDatablockData(d));
      p.ar = clDraLoad(dbcur, 1);
      clDraPush(result, p);
      clDatablockFreePtr(dbcur);
    }
  }
  else {
    result = clCalloc(sizeof(*result), 1);
    result->alloc = ddh->size;
    if (result->alloc < 10)
      result->alloc = 10;
    result->pc = clCalloc(sizeof(result->pc[0]), result->alloc);
    result->size = ddh->size;
    memcpy(result->pc, cur, sizeof(result->pc[0]) * result->size);
    clDraVerify(result);
  }
  return result;
}

struct DataBlock *clDraDump(const struct DRA *d)
{
  if (d == NULL) {
    clLogError("NULL ptr in clDraDump()\n");
  }
  return clDraDeepDump(d, 0);
}

struct DataBlock *clDraDeepDump(const struct DRA *d, int level)
{
  struct DRA *bufs = clDraNew();
  unsigned char *ptr;
  int dbsize;
  struct DataBlock *result;

  struct TagHdr h;
  struct DRAHdr ddh;
  if (d == NULL) {
    clLogError("NULL ptr in clDraDeepDump()\n");
  }
  dbsize = 0;

  ddh.level = level;
  ddh.size = d->size;
  h.tagnum = TAGNUM_DOUBLEDOUBLER;

  if (level) {
    int i;
    for (i = 0; i < d->size; i += 1) {
      union PCTypes p;
      p.dbp = clDraDeepDump(clDraGetValueAt(d, i).ar, level-1);
      clDraPush(bufs, p);
      dbsize += clDatablockSize(p.dbp);
    }
    ptr = clCalloc(dbsize + sizeof(h) + sizeof(ddh), 1);
    h.size = dbsize + sizeof(ddh);
    memcpy(ptr, &h, sizeof(h));
    memcpy(ptr + sizeof(h), &ddh, sizeof(ddh));
    dbsize = sizeof(h) + sizeof(ddh);
    for (i = 0; i < bufs->size; i += 1) {
      struct DataBlock *cur = clDraGetValueAt(bufs, i).dbp;
      memcpy(ptr + dbsize, clDatablockData(cur), clDatablockSize(cur));
      dbsize += clDatablockSize(cur);
      clDatablockFreePtr(cur);
    }
  }
  else {
    dbsize = (d->size * sizeof(d->pc[0])) + sizeof(h) + sizeof(ddh);
    h.size = dbsize - sizeof(h);
    ptr = clCalloc(dbsize, 1);
    memcpy(ptr, &h, sizeof(h));
    memcpy(ptr + sizeof(h), &ddh, sizeof(ddh));
    memcpy(ptr + sizeof(h) + sizeof(ddh), d->pc, d->size * sizeof(d->pc[0]));
  }

  result = clDatablockNewFromBlock(ptr, dbsize);
  clFree(ptr);
  return result;
}

struct DataBlock *clStringDump(const char *s)
{
  struct DataBlock *result;
  unsigned char *ptr;
  struct TagHdr h;
  int dbsize;

  if (s == NULL) {
    clLogError("NULL ptr in clStringDump()\n");
  }

  h.tagnum = TAGNUM_STRING;
  h.size = strlen(s);
  dbsize = h.size + sizeof(h);
  ptr = clCalloc(dbsize,1);
  memcpy(ptr, &h, sizeof(h));
  memcpy(ptr + sizeof(h), s, h.size);
  result = clDatablockNewFromBlock(ptr, dbsize);
  clFree(ptr);
  return result;
}

char *clStringLoad(struct DataBlock *db, int fmustbe)
{
  char *result;
  struct TagHdr *h = (struct TagHdr *) clDatablockData(db);

  if (db == NULL) {
    clLogError("NULL ptr in clStringLoad()\n");
  }
  if (h->tagnum != TAGNUM_STRING) {
    if (fmustbe) {
      clLogError("Error: expecting STRING tagnum %x, got %x\n",
        TAGNUM_STRING,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }
  result = clCalloc(h->size + 1, 1);
  memcpy(result,clDatablockData(db) + sizeof(*h), h->size);
  result[h->size] = '\0';
  return result;
}

int clDraHasQB(const struct DRA *da, qbase_t which)
{
  int i;
  for (i = 0; i < clDraSize(da); i += 1)
    if (which == clDraGetValueAt(da, i).i)
      return 1;
  return 0;
}

void clDraAddQBIfNew(struct DRA *da, qbase_t which)
{
  if (!clDraHasQB(da, which)) {
    union PCTypes p = zeropct;
    p.i = which;
    clDraPush(da, p);
  }
}

