#include <assert.h>
#include "clmalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complearn/doublea.h>
#include <complearn/complearn.h>

const union PCTypes zeropct;
const union PCTypes onepcti = { .i = 1 };
void makeSizeFor(struct DoubleA *da, int where);

struct DoubleA {
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

union PCTypes doubleaRandom(const struct DoubleA *da)
{
  int i;
  assert(da->size > 0);
  i = rand() % da->size;
  return da->pc[i];
}

static void callUserFunc(struct DoubleA *ptr)
{
  if (!ptr->func)
    return;
  ptr->func(ptr, ptr->udata);
  ptr->func = NULL;
  ptr->udata = NULL;
}

void doubleaFree(struct DoubleA *ptr)
{
  assert(ptr);
  callUserFunc(ptr);
  assert(ptr->pc);
  clFreeandclear(ptr->pc);
  ptr->alloc = 0;
  ptr->size = 0;
  clFreeandclear(ptr);
}

struct DoubleA *doubleaNew(void)
{
  struct DoubleA *da = clCalloc(sizeof(struct DoubleA), 1);
  da->alloc = 10;
  da->func = NULL;
  da->elemsize = sizeof(union PCTypes);
  da->pc = clCalloc(da->elemsize, da->alloc);
  return da;
}

int doubleaSize(const struct DoubleA *a)
{
  return a->size;
}

double doubleaGetDValueAt(struct DoubleA *da, int where)
{
  return doubleaGetValueAt(da, where).d;
}

void doubleaVerify(const struct DoubleA *da)
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

union PCTypes doubleaGetValueAt(const struct DoubleA *da, int where)
{
#if LOGICWALL
  assert(da);
  assert(da->pc && "tried to access freed DoubleA");
  doubleaVerify(da);
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

void doubleaSetValueAt(struct DoubleA *da, int where, union PCTypes p)
{
  assert(da);
  assert(where >= 0);
  makeSizeFor(da, where);
  da->pc[where] = p;
  if (where + 1 >= da->size)
    da->size = where + 1;
}

void doubleaSetDValueAt(struct DoubleA *da, int where, double val)
{
  union PCTypes pc;
  pc.d = val;
  doubleaSetValueAt(da, where, pc);
}

void makeSizeFor(struct DoubleA *da, int where)
{
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

union PCTypes doubleaShift(struct DoubleA *da)
{
  union PCTypes result = da->pc[0];
  assert(da->size > 0);
  memmove(da->pc, da->pc+1, (da->size-1) * sizeof(da->pc[0]));
  da->size -= 1;
  return result;
}

union PCTypes doubleaPop(struct DoubleA *da)
{
  union PCTypes result = da->pc[da->size-1];
  assert(da->size > 0);
  memset(da->pc+(da->size-1), 0, sizeof(da->pc[0]));
  da->size -= 1;
  return result;
}

void doubleaUnshift(struct DoubleA *da, union PCTypes p)
{
  assert(da);
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->alloc >= 0);
  assert(da->alloc >= da->size);
  makeSizeFor(da, doubleaSize(da));
  assert(da);
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->alloc >= 0);
  assert(da->alloc >= da->size);
  memmove(da->pc+1, da->pc, sizeof(da->pc[0]) * doubleaSize(da));
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

void doubleaPush(struct DoubleA *da, union PCTypes p)
{
#if LOGICWALL
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->size < 10000);
#endif
  doubleaSetValueAt(da, da->size, p);
}

int doubleaSwapAt(struct DoubleA *da, int inda, int indb)
{
  union PCTypes tmp;
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

struct DoubleA *doubleaClone(const struct DoubleA *ptr)
{
  assert(ptr);
  return doubleaDeepClone(ptr, 0);
}

void doubleaDeepFree(struct DoubleA *ptr, int lvl)
{
  assert(ptr);
  if (lvl) {
    int i;
    int sz = doubleaSize(ptr);
    for (i = 0; i < sz; ++i)
      doubleaDeepFree(doubleaGetValueAt(ptr, i).ar, lvl-1);
  }
  doubleaFree(ptr);
}

struct DoubleA *doubleaDeepClone(const struct DoubleA *ptr, int lvl)
{
  struct DoubleA *result = doubleaNew();
  int sz = doubleaSize(ptr);
  int i;
  for (i = 0; i < sz; ++i) {
    union PCTypes p = zeropct;
    if (lvl)
      p.ar = doubleaDeepClone(doubleaGetValueAt(ptr, i).ar, lvl-1);
    else
      p = doubleaGetValueAt(ptr, i);
    doubleaSetValueAt(result, i, p);
  }
  return result;
}

void doubleaPrintIntList(const struct DoubleA *da)
{
  int i;
  for (i = 0; i < da->size; ++i)
    printf("%d ", da->pc[i].i);
  printf("\n");
}

void doubleaPrintIntPairList(const struct DoubleA *da)
{
  int i;
  for (i = 0; i < da->size; i += 1)
    printf("(%d,%d) ", da->pc[i].ip.x, da->pc[i].ip.y);
  printf("\n");
}

struct DoubleA *doubleaLoad(struct DataBlock *d, int fmustbe)
{
  int i;
  struct TagHdr *h;
  struct DAHdr *ddh;
  struct DoubleA *result;
  unsigned char *cur;
  cur = datablockData(d) + sizeof(*h) + sizeof(*ddh);
  h = (struct TagHdr *) datablockData(d);
  ddh = (struct DAHdr *) (datablockData(d) + sizeof(*h));
  if (h->tagnum != TAGNUM_DOUBLEDOUBLER) {
    clogError("Error: expecting DOUBLEDOUBLER tagnum %x, got %x\n",
        TAGNUM_DOUBLEDOUBLER, h->tagnum);
    exit(1);
  }
  if (ddh->level) {
    result = doubleaNew();
    for (i = 0; i < ddh->size; ++i) {
      union PCTypes p = zeropct;
      struct DataBlock *dbcur;
      dbcur = datablockNewFromBlock(cur,cur+datablockSize(d)-datablockData(d));
      p.ar = doubleaLoad(dbcur, 1);
      doubleaPush(result, p);
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
    doubleaVerify(result);
  }
  return result;
}

struct DataBlock *doubleaDump(const struct DoubleA *d)
{
  return doubleaDeepDump(d, 0);
}

struct DataBlock *doubleaDeepDump(const struct DoubleA *d, int level)
{
  struct DoubleA *bufs = doubleaNew();
  unsigned char *ptr;
  int dbsize;
  struct DataBlock *result;

  struct TagHdr h;
  struct DAHdr ddh;
  dbsize = 0;

  ddh.level = level;
  ddh.size = d->size;
  h.tagnum = TAGNUM_DOUBLEDOUBLER;

  if (level) {
    int i;
    for (i = 0; i < d->size; i += 1) {
      union PCTypes p;
      p.dbp = doubleaDeepDump(doubleaGetValueAt(d, i).ar, level-1);
      doubleaPush(bufs, p);
      dbsize += datablockSize(p.dbp);
    }
    ptr = clCalloc(dbsize + sizeof(h) + sizeof(ddh), 1);
    h.size = dbsize + sizeof(ddh);
    memcpy(ptr, &h, sizeof(h));
    memcpy(ptr + sizeof(h), &ddh, sizeof(ddh));
    dbsize = sizeof(h) + sizeof(ddh);
    for (i = 0; i < bufs->size; i += 1) {
      struct DataBlock *cur = doubleaGetValueAt(bufs, i).dbp;
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

int doubleaHasQB(const struct DoubleA *da, qbase_t which)
{
  int i;
  if (da == NULL) {
    clogError("NULL ptr in doubleaHasQB()\n");
  }
  for (i = 0; i < doubleaSize(da); i += 1)
    if (which == doubleaGetValueAt(da, i).i)
      return 1;
  return 0;
}

void doubleaAddQBIfNew(struct DoubleA *da, qbase_t which)
{
  if (da == NULL) {
    clogError("NULL ptr in doubleaAddQBIfNew()\n");
  }
  if (!doubleaHasQB(da, which)) {
    union PCTypes p = zeropct;
    p.i = which;
    doubleaPush(da, p);
  }
}

