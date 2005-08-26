#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <complearn/complearn.h>

struct StringStack {
  struct DoubleA *da;
};

struct StringStack *loadStringStack(struct DataBlock db, int fmustbe)
{
  struct StringStack *result = newStringStack();
  struct DataBlock cur;
  struct TagManager *tm;
  struct TagHdr *h = (struct TagHdr *) db.ptr;

  if (h->tagnum != TAGNUM_STRINGSTACK) {
    if (fmustbe) {
      fprintf(stderr,"Error: expecting STRINGSTACK tagnum %x, got %x\n",
          TAGNUM_STRINGSTACK,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }

  tm = newTagManager(db);

  while (getCurDataBlock(tm, &cur)) {
    char *str;
    str = loadString(cur, 1);
    pushSS(result, str);
    gfreeandclear(str);
    stepNextDataBlock(tm);
  }

  freeTagManager(tm);
  return result;
}

struct DataBlock dumpStringStack(const struct StringStack *ss)
{
  struct DataBlock result;
  struct DoubleA *parts = newDoubleDoubler();
  int i;

  for ( i = 0; i < sizeSS(ss); i += 1) {
    union PCTypes p = zeropct;
    char *s = getValueAt(ss->da,i).str;
    p.db = dumpString(s);
    pushValue(parts,p);
  }

  result = package_dd_DataBlocks(TAGNUM_STRINGSTACK, parts);
  freeDoubleDoubler(parts);
  return result;
}

struct StringStack *newSingleSS(const char *str)
{
	struct StringStack *ss;
  ss = newStringStack();
  pushSS(ss, str);
  return ss;
}

struct StringStack *newStringStack()
{
	struct StringStack *ss;
	ss = (struct StringStack*)gcalloc(sizeof(struct StringStack), 1);
  ss->da = newDoubleDoubler();
	// ss->size = 0;
	return ss;
}

struct StringStack *cloneSS(struct StringStack *ss)
{
  struct StringStack *nss;
  int i, sz;
  sz = sizeSS(ss);
  nss = newStringStack();
  for (i = 0; i < sz; ++i) {
    union PCTypes p;
    p.str = gstrdup(getValueAt(ss->da, i).str);
    setValueAt(nss->da, i, p);
  }
  return nss;
}

int freeSS(struct StringStack *ss)
{
  int i;
  for (i = 0; i < getSize(ss->da); i += 1) {
    gfree(getValueAt(ss->da, i).str);
  }
  freeDoubleDoubler(ss->da);
  ss->da = NULL;
	gfreeandclear(ss);
	return CL_OK;
}

int unshiftSS(struct StringStack *ss, const char *str)
{
  union PCTypes p;
  assert(ss);
  assert(str);
  p.str = gstrdup(str);
  unshiftValue(ss->da, p);
  return CL_OK;
}

int pushSS(struct StringStack *ss, const char *str)
{
  union PCTypes p;
  assert(ss);
  assert(str);
  p.str = gstrdup(str);
  pushValue(ss->da, p);
  return CL_OK;
}

int isEmptySS(struct StringStack *ss)
{
	return getSize(ss->da) == 0;
}

int sizeSS(const struct StringStack *ss)
{
	return getSize(ss->da);
}

/* After calling shiftSS, it is the responsibility of the programmer to free
 * the shifted string
 */
char *shiftSS(struct StringStack *ss)
{
  union PCTypes p;
  assert(sizeSS(ss) > 0);
  memset(&p, 0, sizeof(p));
	if (sizeSS(ss) == 0) return p.str;
	p = shiftDoubleDoubler(ss->da);
  return p.str;
}

/* After calling popSS, it is the responsibility of the programmer to free
 * the popped string
 */
char *popSS(struct StringStack *ss)
{
  union PCTypes p;
  memset(&p, 0, sizeof(p));
	if (sizeSS(ss) == 0) return p.str;
	p = popDoubleDoubler(ss->da);
  return p.str;
}

char *readAtSS(struct StringStack *ss, int i)
{
	return getValueAt(ss->da, i).str;
}

int sortSS(struct StringStack *ss)
{
  int flipped, i;
  int sz = sizeSS(ss);
  do {
    flipped = 0;
    for (i = 1; i < sz; ++i) {
      if (strcmp(getValueAt(ss->da, i-1).str, getValueAt(ss->da, i).str) > 0) {
        swapValues(ss->da, i-1, i);
        flipped = 1;
      }
    }
  } while (flipped);
  return CL_OK;
}

struct StringStack *mergeSS(struct StringStack *ssa, struct StringStack *ssb)
{
  struct StringStack *result;
  int i;
  result = cloneSS(ssa);
  for (i = 0; i < sizeSS(ssb); i += 1)
    pushSS(result, readAtSS(ssb, i));
  return result;
}

void printSS(struct StringStack *ss)
{
  int i;
  for (i = 0; i < sizeSS(ss); i += 1)
    printf("%s\n", readAtSS(ss, i));
}

