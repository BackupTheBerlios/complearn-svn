#include "clalloc.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <complearn/complearn.h>

struct StringStack {
  struct DRA *da;
};

struct StringStack *clStringstackLoad(struct DataBlock *db, int fmustbe)
{
  struct StringStack *result = clStringstackNew();
  struct DataBlock *cur = NULL;
  struct TagManager *tm;
  struct TagHdr *h = (struct TagHdr *) clDatablockData(db);

  if (h->tagnum != TAGNUM_STRINGSTACK) {
    if (fmustbe) {
      clogError("Error: expecting STRINGSTACK tagnum %x, got %x\n",
          TAGNUM_STRINGSTACK,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }

  tm = clNewTagManager(db);

  while ((cur = clGetCurDataBlock(tm))) {
    char *str;
    str = clStringLoad(cur, 1);
    clStringstackPush(result, str);
    clFreeandclear(str);
    clStepNextDataBlock(tm);
    clDatablockFreePtr(cur);
  }

  clFreeTagManager(tm);
  return result;
}

struct DataBlock *clStringstackDump(const struct StringStack *ss)
{
  struct DataBlock *result;
  struct DRA *parts = clDraNew();
  int i;

  for ( i = 0; i < clStringstackSize(ss); i += 1) {
    union PCTypes p = zeropct;
    char *s = clDraGetValueAt(ss->da,i).str;
    p.dbp = clStringDump(s); // TODO: fix mem leak here
    clDraPush(parts,p);
  }

  result = clPackage_dd_DataBlocks(TAGNUM_STRINGSTACK, parts);
  clDraFree(parts);
  return result;
}

struct StringStack *clStringstackNewSingle(const char *str)
{
	struct StringStack *ss;
  ss = clStringstackNew();
  clStringstackPush(ss, str);
  return ss;
}

struct StringStack *clStringstackNew()
{
	struct StringStack *ss;
	ss = (struct StringStack*)clCalloc(sizeof(struct StringStack), 1);
  ss->da = clDraNew();
	// ss->size = 0;
	return ss;
}

struct StringStack *clStringstackClone(struct StringStack *ss)
{
  struct StringStack *nss;
  int i, sz;
  sz = clStringstackSize(ss);
  nss = clStringstackNew();
  for (i = 0; i < sz; ++i) {
    union PCTypes p;
    p.str = clStrdup(clDraGetValueAt(ss->da, i).str);
    clDraSetValueAt(nss->da, i, p);
  }
  return nss;
}

int clStringstackFree(struct StringStack *ss)
{
  int i;
  for (i = 0; i < clDraSize(ss->da); i += 1) {
    clFree(clDraGetValueAt(ss->da, i).str);
  }
  clDraFree(ss->da);
  ss->da = NULL;
	clFreeandclear(ss);
	return CL_OK;
}

int clStringstackUnshift(struct StringStack *ss, const char *str)
{
  union PCTypes p;
  assert(ss);
  assert(str);
  p.str = clStrdup(str);
  clDraUnshift(ss->da, p);
  return CL_OK;
}

int clStringstackPush(struct StringStack *ss, const char *str)
{
  union PCTypes p;
  assert(ss);
  assert(str);
  p.str = clStrdup(str);
  clDraPush(ss->da, p);
  return CL_OK;
}

int clStringstackIsEmpty(struct StringStack *ss)
{
	return clDraSize(ss->da) == 0;
}

int clStringstackSize(const struct StringStack *ss)
{
	return clDraSize(ss->da);
}

/* After calling clShiftSS, it is the responsibility of the programmer to free
 * the shifted string
 */
char *clShiftSS(struct StringStack *ss)
{
  union PCTypes p;
  assert(clStringstackSize(ss) > 0);
  memset(&p, 0, sizeof(p));
	if (clStringstackSize(ss) == 0) return p.str;
	p = clDraShift(ss->da);
  return p.str;
}

/* After calling clStringstackPop, it is the responsibility of the programmer to free
 * the popped string
 */
char *clStringstackPop(struct StringStack *ss)
{
  union PCTypes p;
  memset(&p, 0, sizeof(p));
	if (clStringstackSize(ss) == 0) return p.str;
	p = clDraPop(ss->da);
  return p.str;
}

char *clStringstackReadAt(struct StringStack *ss, int i)
{
	return clDraGetValueAt(ss->da, i).str;
}

int clStringstackSort(struct StringStack *ss)
{
  int flipped, i;
  int sz = clStringstackSize(ss);
  do {
    flipped = 0;
    for (i = 1; i < sz; ++i) {
      if (strcmp(clDraGetValueAt(ss->da, i-1).str, clDraGetValueAt(ss->da, i).str) > 0) {
        clDraSwapAt(ss->da, i-1, i);
        flipped = 1;
      }
    }
  } while (flipped);
  return CL_OK;
}

struct StringStack *clStringstackMerge(struct StringStack *ssa, struct StringStack *ssb)
{
  struct StringStack *result;
  int i;
  result = clStringstackClone(ssa);
  for (i = 0; i < clStringstackSize(ssb); i += 1)
    clStringstackPush(result, clStringstackReadAt(ssb, i));
  return result;
}

void clStringstackPrint(struct StringStack *ss)
{
  int i;
  for (i = 0; i < clStringstackSize(ss); i += 1)
    printf("%s\n", clStringstackReadAt(ss, i));
}

