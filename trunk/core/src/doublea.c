#include <assert.h>
#include "clalloc.h"
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
  t_doublefreefunc func;
  void *udata;
  union PCTypes *pc;
};

/* The basic file header */

struct DAHdr {
  int level;
  int size;
};

union PCTypes draRandom(const struct DRA *da)
{
  int i;
  if (da == NULL) {
    clogError("NULL ptr in draRandom()\n");
  }
  assert(da->size > 0);
  i = rand() % da->size;
  return da->pc[i];
}

static void callUserFunc(struct DRA *ptr)
{
  if (!ptr->func)
    return;
  ptr->func(ptr, ptr->udata);
  ptr->func = NULL;
  ptr->udata = NULL;
}

void draFree(struct DRA *ptr)
{
  assert(ptr);
  callUserFunc(ptr);
  assert(ptr->pc);
  clFreeandclear(ptr->pc);
  ptr->alloc = 0;
  ptr->size = 0;
  clFreeandclear(ptr);
}

struct DRA *draNew(void)
{
  struct DRA *da = clCalloc(sizeof(struct DRA), 1);
  da->alloc = 10;
  da->func = NULL;
  da->elemsize = sizeof(union PCTypes);
  da->pc = clCalloc(da->elemsize, da->alloc);
  return da;
}

int draSize(const struct DRA *a)
{
  if (a == NULL) {
    clogError("NULL ptr in draSize()\n");
  }
  return a->size;
}

double draGetDValueAt(struct DRA *da, int where)
{
  if (da == NULL) {
    clogError("NULL ptr in draGetDValueAt()\n");
  }
  return draGetValueAt(da, where).d;
}

void draVerify(const struct DRA *da)
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

union PCTypes draGetValueAt(const struct DRA *da, int where)
{
#if LOGICWALL
  if (da == NULL) {
    clogError("NULL ptr in draGetValueAt()\n");
  }
  assert(da);
  assert(da->pc && "tried to access freed DRA");
  draVerify(da);
  assert(where >= 0);
  assert(where < 1000000);  /* TODO: remove me */
  if (where >= da->size) {
    clogError( "Error, trying to read element index %d but only of size %d\n",
        where, da->size);
    assert(where < da->size && "reading unitialized element in array");
  }
#endif
  return da->pc[where];
}

void draSetValueAt(struct DRA *da, int where, union PCTypes p)
{
  if (da == NULL) {
    clogError("NULL ptr in draSetValueAt()\n");
  }
  assert(da);
  assert(where >= 0);
  makeSizeFor(da, where);
  da->pc[where] = p;
  if (where + 1 >= da->size)
    da->size = where + 1;
}

void draSetDValueAt(struct DRA *da, int where, double val)
{
  union PCTypes pc;
  if (da == NULL) {
    clogError("NULL ptr in draSetDValueAt()\n");
  }
  pc.d = val;
  draSetValueAt(da, where, pc);
}

static void makeSizeFor(struct DRA *da, int where)
{
  if (da == NULL) {
    clogError("NULL ptr in makeSizeFor()\n");
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

union PCTypes draShift(struct DRA *da)
{
  union PCTypes result;
  if (da == NULL) {
    clogError("NULL ptr in draShift()\n");
  }
  result = da->pc[0];
  assert(da->size > 0);
  memmove(da->pc, da->pc+1, (da->size-1) * sizeof(da->pc[0]));
  da->size -= 1;
  return result;
}

union PCTypes draPop(struct DRA *da)
{
  union PCTypes result;
  if (da == NULL) {
    clogError("NULL ptr in draPop()\n");
  }
  result  = da->pc[da->size-1];
  assert(da->size > 0);
  memset(da->pc+(da->size-1), 0, sizeof(da->pc[0]));
  da->size -= 1;
  return result;
}

void draUnshift(struct DRA *da, union PCTypes p)
{
  if (da == NULL) {
    clogError("NULL ptr in draUnshift()\n");
  }
  assert(da);
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->alloc >= 0);
  assert(da->alloc >= da->size);
  makeSizeFor(da, draSize(da));
  assert(da);
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->alloc >= 0);
  assert(da->alloc >= da->size);
  memmove(da->pc+1, da->pc, sizeof(da->pc[0]) * draSize(da));
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

void draPush(struct DRA *da, union PCTypes p)
{
  if (da == NULL) {
    clogError("NULL ptr in draPush()\n");
  }
#if LOGICWALL
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->size < 10000);
#endif
  draSetValueAt(da, da->size, p);
}

int draSwapAt(struct DRA *da, int inda, int indb)
{
  union PCTypes tmp;
  if (da == NULL) {
    clogError("NULL ptr in draSwapAt()\n");
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

struct DRA *draClone(const struct DRA *ptr)
{
  if (ptr == NULL) {
    clogError("NULL ptr in draClone()\n");
  }
  assert(ptr);
  return draDeepClone(ptr, 0);
}

void draDeepFree(struct DRA *ptr, int lvl)
{
  if (ptr == NULL) {
    clogError("NULL ptr in draDeepFree()\n");
  }
  assert(ptr);
  if (lvl) {
    int i;
    int sz = draSize(ptr);
    for (i = 0; i < sz; ++i)
      draDeepFree(draGetValueAt(ptr, i).ar, lvl-1);
  }
  draFree(ptr);
}

struct DRA *draDeepClone(const struct DRA *ptr, int lvl)
{
  struct DRA *result = draNew();
  int sz;
  int i;
  if (ptr == NULL) {
    clogError("NULL ptr in draDeepClone()\n");
  }
  sz = draSize(ptr);
  for (i = 0; i < sz; ++i) {
    union PCTypes p = zeropct;
    if (lvl)
      p.ar = draDeepClone(draGetValueAt(ptr, i).ar, lvl-1);
    else
      p = draGetValueAt(ptr, i);
    draSetValueAt(result, i, p);
  }
  return result;
}

void draPrintIntList(const struct DRA *da)
{
  int i;
  if (da == NULL) {
    clogError("NULL ptr in draPrintIntList()\n");
  }
  for (i = 0; i < da->size; ++i)
    printf("%d ", da->pc[i].i);
  printf("\n");
}

void draPrintIntPairList(const struct DRA *da)
{
  int i;
  if (da == NULL) {
    clogError("NULL ptr in draPrintIntPairList()\n");
  }
  for (i = 0; i < da->size; i += 1)
    printf("(%d,%d) ", da->pc[i].ip.x, da->pc[i].ip.y);
  printf("\n");
}

struct DRA *draLoad(struct DataBlock *d, int fmustbe)
{
  int i;
  struct TagHdr *h;
  struct DAHdr *ddh;
  struct DRA *result;
  unsigned char *cur;
  if (d == NULL) {
    clogError("NULL ptr in draLoad()\n");
  }
  cur = datablockData(d) + sizeof(*h) + sizeof(*ddh);
  h = (struct TagHdr *) datablockData(d);
  ddh = (struct DAHdr *) (datablockData(d) + sizeof(*h));
  if (h->tagnum != TAGNUM_DOUBLEDOUBLER) {
    clogError("Error: expecting DOUBLEDOUBLER tagnum %x, got %x\n",
        TAGNUM_DOUBLEDOUBLER, h->tagnum);
    exit(1);
  }
  if (ddh->level) {
    result = draNew();
    for (i = 0; i < ddh->size; ++i) {
      union PCTypes p = zeropct;
      struct DataBlock *dbcur;
      dbcur = datablockNewFromBlock(cur,cur+datablockSize(d)-datablockData(d));
      p.ar = draLoad(dbcur, 1);
      draPush(result, p);
      datablockFreePtr(dbcur);
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
    draVerify(result);
  }
  return result;
}

struct DataBlock *draDump(const struct DRA *d)
{
  if (d == NULL) {
    clogError("NULL ptr in draDump()\n");
  }
  return draDeepDump(d, 0);
}

struct DataBlock *draDeepDump(const struct DRA *d, int level)
{
  struct DRA *bufs = draNew();
  unsigned char *ptr;
  int dbsize;
  struct DataBlock *result;

  struct TagHdr h;
  struct DAHdr ddh;
  if (d == NULL) {
    clogError("NULL ptr in draDeepDump()\n");
  }
  dbsize = 0;

  ddh.level = level;
  ddh.size = d->size;
  h.tagnum = TAGNUM_DOUBLEDOUBLER;

  if (level) {
    int i;
    for (i = 0; i < d->size; i += 1) {
      union PCTypes p;
      p.dbp = draDeepDump(draGetValueAt(d, i).ar, level-1);
      draPush(bufs, p);
      dbsize += datablockSize(p.dbp);
    }
    ptr = clCalloc(dbsize + sizeof(h) + sizeof(ddh), 1);
    h.size = dbsize + sizeof(ddh);
    memcpy(ptr, &h, sizeof(h));
    memcpy(ptr + sizeof(h), &ddh, sizeof(ddh));
    dbsize = sizeof(h) + sizeof(ddh);
    for (i = 0; i < bufs->size; i += 1) {
      struct DataBlock *cur = draGetValueAt(bufs, i).dbp;
      memcpy(ptr + dbsize, datablockData(cur), datablockSize(cur));
      dbsize += datablockSize(cur);
      datablockFreePtr(cur);
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

  result = datablockNewFromBlock(ptr, dbsize);
  clFree(ptr);
  return result;
}

struct DataBlock *stringDump(const char *s)
{
  struct DataBlock *result;
  unsigned char *ptr;
  struct TagHdr h;
  int dbsize;

  if (s == NULL) {
    clogError("NULL ptr in stringDump()\n");
  }

  h.tagnum = TAGNUM_STRING;
  h.size = strlen(s);
  dbsize = h.size + sizeof(h);
  ptr = clCalloc(dbsize,1);
  memcpy(ptr, &h, sizeof(h));
  memcpy(ptr + sizeof(h), s, h.size);
  result = datablockNewFromBlock(ptr, dbsize);
  clFree(ptr);
  return result;
}

char *stringLoad(struct DataBlock *db, int fmustbe)
{
  char *result;
  struct TagHdr *h = (struct TagHdr *) datablockData(db);

  if (db == NULL) {
    clogError("NULL ptr in stringLoad()\n");
  }
  if (h->tagnum != TAGNUM_STRING) {
    if (fmustbe) {
      clogError("Error: expecting STRING tagnum %x, got %x\n",
        TAGNUM_STRING,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }
  result = clCalloc(h->size + 1, 1);
  memcpy(result,datablockData(db) + sizeof(*h), h->size);
  result[h->size] = '\0';
  return result;
}

int draHasQB(const struct DRA *da, qbase_t which)
{
  int i;
  for (i = 0; i < draSize(da); i += 1)
    if (which == draGetValueAt(da, i).i)
      return 1;
  return 0;
}

void draAddQBIfNew(struct DRA *da, qbase_t which)
{
  if (!draHasQB(da, which)) {
    union PCTypes p = zeropct;
    p.i = which;
    draPush(da, p);
  }
}

