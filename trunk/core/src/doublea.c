#include <assert.h>
#include <malloc.h>
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

union PCTypes getRandomElement(const struct DoubleA *da)
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

void freeDoubleDoubler(struct DoubleA *ptr)
{
  assert(ptr);
  callUserFunc(ptr);
  assert(ptr->pc);
  gfreeandclear(ptr->pc);
  ptr->alloc = 0;
  ptr->size = 0;
  gfreeandclear(ptr);
}

struct DoubleA *doubleaNew(void)
{
  struct DoubleA *da = gcalloc(sizeof(struct DoubleA), 1);
  da->alloc = 10;
  da->func = NULL;
  da->elemsize = sizeof(union PCTypes);
  da->pc = gcalloc(da->elemsize, da->alloc);
  return da;
}

int getSize(const struct DoubleA *a)
{
  return a->size;
}

double getDValueAt(struct DoubleA *da, int where)
{
  return getValueAt(da, where).d;
}

void verifyDoubleDoubler(const struct DoubleA *da)
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

union PCTypes getValueAt(const struct DoubleA *da, int where)
{
#if LOGICWALL
  assert(da);
  assert(da->pc && "tried to access freed DoubleA");
  verifyDoubleDoubler(da);
  assert(where >= 0);
  assert(where < 1000000);  /* TODO: remove me */
  if (where >= da->size) {
    fprintf(stderr, "Error, trying to read element index %d but only of size %d\n",
        where, da->size);
    assert(where < da->size && "reading unitialized element in array");
  }
#endif
  return da->pc[where];
}

void setValueAt(struct DoubleA *da, int where, union PCTypes p)
{
  assert(da);
  assert(where >= 0);
  makeSizeFor(da, where);
  da->pc[where] = p;
  if (where + 1 >= da->size)
    da->size = where + 1;
}

void setDValueAt(struct DoubleA *da, int where, double val)
{
  union PCTypes pc;
  pc.d = val;
  setValueAt(da, where, pc);
}

void makeSizeFor(struct DoubleA *da, int where)
{
  while (where >= da->alloc) {
    union PCTypes *newBlock;
    int newAlloc = da->alloc * 2;
    newBlock = gcalloc(da->elemsize, newAlloc);
    assert(newBlock);
    memcpy(newBlock, da->pc, da->elemsize * da->alloc);
    gfreeandclear(da->pc);
    da->pc = newBlock;
    da->alloc = newAlloc;
  }
}

union PCTypes shiftDoubleDoubler(struct DoubleA *da)
{
  union PCTypes result = da->pc[0];
  assert(da->size > 0);
  memmove(da->pc, da->pc+1, (da->size-1) * sizeof(da->pc[0]));
  da->size -= 1;
  return result;
}

union PCTypes popDoubleDoubler(struct DoubleA *da)
{
  union PCTypes result = da->pc[da->size-1];
  assert(da->size > 0);
  memset(da->pc+(da->size-1), 0, sizeof(da->pc[0]));
  da->size -= 1;
  return result;
}

void unshiftValue(struct DoubleA *da, union PCTypes p)
{
  assert(da);
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->alloc >= 0);
  assert(da->alloc >= da->size);
  makeSizeFor(da, getSize(da));
  assert(da);
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->alloc >= 0);
  assert(da->alloc >= da->size);
  memmove(da->pc+1, da->pc, sizeof(da->pc[0]) * getSize(da));
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

void pushValue(struct DoubleA *da, union PCTypes p)
{
#if LOGICWALL
  assert(da->size >= 0);
  assert(da->pc);
  assert(da->size < 10000);
#endif
  setValueAt(da, da->size, p);
}

int swapValues(struct DoubleA *da, int inda, int indb)
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

struct DoubleA *cloneDoubler(const struct DoubleA *ptr)
{
  assert(ptr);
  return deepCopyLvl(ptr, 0);
}

void freeDeepDoubleDoubler(struct DoubleA *ptr, int lvl)
{
  assert(ptr);
  if (lvl) {
    int i;
    int sz = getSize(ptr);
    for (i = 0; i < sz; ++i)
      freeDeepDoubleDoubler(getValueAt(ptr, i).ar, lvl-1);
  }
  freeDoubleDoubler(ptr);
}

struct DoubleA *deepCopyLvl(const struct DoubleA *ptr, int lvl)
{
  struct DoubleA *result = doubleaNew();
  int sz = getSize(ptr);
  int i;
  for (i = 0; i < sz; ++i) {
    union PCTypes p = zeropct;
    if (lvl)
      p.ar = deepCopyLvl(getValueAt(ptr, i).ar, lvl-1);
    else
      p = getValueAt(ptr, i);
    setValueAt(result, i, p);
  }
  return result;
}

void printNodeList(const struct DoubleA *da)
{
  int i;
  for (i = 0; i < da->size; ++i)
    printf("%d ", da->pc[i].i);
  printf("\n");
}

void printIntPairList(const struct DoubleA *da)
{
  int i;
  for (i = 0; i < da->size; i += 1)
    printf("(%d,%d) ", da->pc[i].ip.x, da->pc[i].ip.y);
  printf("\n");
}

struct DoubleA *loadDoubleDoubler(struct DataBlock d, int fmustbe)
{
  int i;
  struct TagHdr *h;
  struct DAHdr *ddh;
  struct DoubleA *result;
  unsigned char *cur;
  cur = d.ptr + sizeof(*h) + sizeof(*ddh);
  h = (struct TagHdr *) d.ptr;
  ddh = (struct DAHdr *) (d.ptr + sizeof(*h));
  if (h->tagnum != TAGNUM_DOUBLEDOUBLER) {
    fprintf(stderr,"Error: expecting DOUBLEDOUBLER tagnum %x, got %x\n",
        TAGNUM_DOUBLEDOUBLER, h->tagnum);
    exit(1);
  }
  if (ddh->level) {
    result = doubleaNew();
    for (i = 0; i < ddh->size; ++i) {
      union PCTypes p = zeropct;
      struct DataBlock dbcur;
      dbcur.ptr = cur;
      dbcur.size = cur + d.size - d.ptr;
      p.ar = loadDoubleDoubler(dbcur, 1);
      pushValue(result, p);
    }
  }
  else {
    result = gcalloc(sizeof(*result), 1);
    result->alloc = ddh->size;
    if (result->alloc < 10)
      result->alloc = 10;
    result->pc = gcalloc(sizeof(result->pc[0]), result->alloc);
    result->size = ddh->size;
    memcpy(result->pc, cur, sizeof(result->pc[0]) * result->size);
    verifyDoubleDoubler(result);
  }
  return result;
}

struct DataBlock dumpDoubleDoubler(const struct DoubleA *d)
{
  return dumpDeepDoubleDoubler(d, 0);
}

struct DataBlock dumpDeepDoubleDoubler(const struct DoubleA *d, int level)
{
  struct DoubleA *bufs = doubleaNew();
  struct DataBlock dbres;

  struct TagHdr h;
  struct DAHdr ddh;
  dbres.size = 0;

  ddh.level = level;
  ddh.size = d->size;
  h.tagnum = TAGNUM_DOUBLEDOUBLER;

  if (level) {
    int i;
    for (i = 0; i < d->size; i += 1) {
      union PCTypes p;
      p.db = dumpDeepDoubleDoubler(getValueAt(d, i).ar, level-1);
      pushValue(bufs, p);
      dbres.size += p.db.size;
    }
    dbres.ptr = gcalloc(dbres.size + sizeof(h) + sizeof(ddh), 1);
    h.size = dbres.size + sizeof(ddh);
    memcpy(dbres.ptr, &h, sizeof(h));
    memcpy(dbres.ptr + sizeof(h), &ddh, sizeof(ddh));
    dbres.size = sizeof(h) + sizeof(ddh);
    for (i = 0; i < bufs->size; i += 1) {
      struct DataBlock cur = getValueAt(bufs, i).db;
      memcpy(dbres.ptr + dbres.size, cur.ptr, cur.size);
      dbres.size += cur.size;
      datablockFree(cur);
    }
  }
  else {
    dbres.size = (d->size * sizeof(d->pc[0])) + sizeof(h) + sizeof(ddh);
    h.size = dbres.size - sizeof(h);
    dbres.ptr = gcalloc(dbres.size, 1);
    memcpy(dbres.ptr, &h, sizeof(h));
    memcpy(dbres.ptr + sizeof(h), &ddh, sizeof(ddh));
    memcpy(dbres.ptr + sizeof(h) + sizeof(ddh), d->pc, d->size * sizeof(d->pc[0]));
  }
  freeDoubleDoubler(bufs);
  return dbres;
}

struct DataBlock dumpString(const char *s)
{
  struct DataBlock result;
  struct TagHdr h;

  h.tagnum = TAGNUM_STRING;
  h.size = strlen(s);
  result.size = h.size + sizeof(h);
  result.ptr = gcalloc(result.size,1);
  memcpy(result.ptr, &h, sizeof(h));
  memcpy(result.ptr + sizeof(h), s, h.size);
  return result;
}

char *loadString(struct DataBlock db, int fmustbe)
{
  char *result;
  struct TagHdr *h = (struct TagHdr *) db.ptr;

  if (h->tagnum != TAGNUM_STRING) {
    if (fmustbe) {
      fprintf(stderr,"Error: expecting STRING tagnum %x, got %x\n",
        TAGNUM_STRING,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }
  result = gcalloc(h->size + 1, 1);
  memcpy(result,db.ptr + sizeof(*h), h->size);
  result[h->size] = '\0';
  return result;
}

int isInDAqb(const struct DoubleA *da, qbase_t which)
{
  int i;
  for (i = 0; i < getSize(da); i += 1)
    if (which == getValueAt(da, i).i)
      return 1;
  return 0;
}

void addIfNewqb(struct DoubleA *da, qbase_t which)
{
  if (!isInDAqb(da, which)) {
    union PCTypes p = zeropct;
    p.i = which;
    pushValue(da, p);
  }
}

