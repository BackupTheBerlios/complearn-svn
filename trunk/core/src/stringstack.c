#include "clalloc.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <complearn/complearn.h>

struct StringStack {
  struct DRA *da;
};

struct StringStack *stringstackLoad(struct DataBlock *db, int fmustbe)
{
  struct StringStack *result = stringstackNew();
  struct DataBlock *cur = NULL;
  struct TagManager *tm;
  struct TagHdr *h = (struct TagHdr *) datablockData(db);

  if (h->tagnum != TAGNUM_STRINGSTACK) {
    if (fmustbe) {
      clogError("Error: expecting STRINGSTACK tagnum %x, got %x\n",
          TAGNUM_STRINGSTACK,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }

  tm = newTagManager(db);

  while ((cur = getCurDataBlock(tm))) {
    char *str;
    str = stringLoad(cur, 1);
    stringstackPush(result, str);
    clFreeandclear(str);
    stepNextDataBlock(tm);
    datablockFreePtr(cur);
  }

  freeTagManager(tm);
  return result;
}

struct DataBlock *stringstackDump(const struct StringStack *ss)
{
  struct DataBlock *result;
  struct DRA *parts = draNew();
  int i;

  for ( i = 0; i < stringstackSize(ss); i += 1) {
    union PCTypes p = zeropct;
    char *s = draGetValueAt(ss->da,i).str;
    p.dbp = stringDump(s); // TODO: fix mem leak here
    draPush(parts,p);
  }

  result = package_dd_DataBlocks(TAGNUM_STRINGSTACK, parts);
  draFree(parts);
  return result;
}

struct StringStack *stringstackNewSingle(const char *str)
{
	struct StringStack *ss;
  ss = stringstackNew();
  stringstackPush(ss, str);
  return ss;
}

struct StringStack *stringstackNew()
{
	struct StringStack *ss;
	ss = (struct StringStack*)clCalloc(sizeof(struct StringStack), 1);
  ss->da = draNew();
	// ss->size = 0;
	return ss;
}

struct StringStack *stringstackClone(struct StringStack *ss)
{
  struct StringStack *nss;
  int i, sz;
  sz = stringstackSize(ss);
  nss = stringstackNew();
  for (i = 0; i < sz; ++i) {
    union PCTypes p;
    p.str = clStrdup(draGetValueAt(ss->da, i).str);
    draSetValueAt(nss->da, i, p);
  }
  return nss;
}

int stringstackFree(struct StringStack *ss)
{
  int i;
  for (i = 0; i < draSize(ss->da); i += 1) {
    clFree(draGetValueAt(ss->da, i).str);
  }
  draFree(ss->da);
  ss->da = NULL;
	clFreeandclear(ss);
	return CL_OK;
}

int stringstackUnshift(struct StringStack *ss, const char *str)
{
  union PCTypes p;
  assert(ss);
  assert(str);
  p.str = clStrdup(str);
  draUnshift(ss->da, p);
  return CL_OK;
}

int stringstackPush(struct StringStack *ss, const char *str)
{
  union PCTypes p;
  assert(ss);
  assert(str);
  p.str = clStrdup(str);
  draPush(ss->da, p);
  return CL_OK;
}

int stringstackIsEmpty(struct StringStack *ss)
{
	return draSize(ss->da) == 0;
}

int stringstackSize(const struct StringStack *ss)
{
	return draSize(ss->da);
}

/* After calling shiftSS, it is the responsibility of the programmer to free
 * the shifted string
 */
char *shiftSS(struct StringStack *ss)
{
  union PCTypes p;
  assert(stringstackSize(ss) > 0);
  memset(&p, 0, sizeof(p));
	if (stringstackSize(ss) == 0) return p.str;
	p = draShift(ss->da);
  return p.str;
}

/* After calling stringstackPop, it is the responsibility of the programmer to free
 * the popped string
 */
char *stringstackPop(struct StringStack *ss)
{
  union PCTypes p;
  memset(&p, 0, sizeof(p));
	if (stringstackSize(ss) == 0) return p.str;
	p = draPop(ss->da);
  return p.str;
}

char *stringstackReadAt(struct StringStack *ss, int i)
{
	return draGetValueAt(ss->da, i).str;
}

int stringstackSort(struct StringStack *ss)
{
  int flipped, i;
  int sz = stringstackSize(ss);
  do {
    flipped = 0;
    for (i = 1; i < sz; ++i) {
      if (strcmp(draGetValueAt(ss->da, i-1).str, draGetValueAt(ss->da, i).str) > 0) {
        draSwapAt(ss->da, i-1, i);
        flipped = 1;
      }
    }
  } while (flipped);
  return CL_OK;
}

struct StringStack *stringstackMerge(struct StringStack *ssa, struct StringStack *ssb)
{
  struct StringStack *result;
  int i;
  result = stringstackClone(ssa);
  for (i = 0; i < stringstackSize(ssb); i += 1)
    stringstackPush(result, stringstackReadAt(ssb, i));
  return result;
}

void stringstackPrint(struct StringStack *ss)
{
  int i;
  for (i = 0; i < stringstackSize(ss); i += 1)
    printf("%s\n", stringstackReadAt(ss, i));
}

