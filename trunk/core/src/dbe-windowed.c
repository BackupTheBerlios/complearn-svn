#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#include <complearn/complearn.h>

/** \brief Private enumeration instance for windowed enumeration.
 *  \struct DBEWindowedEnumeration
 */
struct DBEWindowedEnumeration
{
  struct DataBlock *db;
  int firstpos, stepsize, width, lastpos;
};

/** \brief Iterator for a DBEWindowedEnumeration.
 *  \struct DBEWindowedEnumerationIterator
 */
struct DBEWindowedEnumerationIterator
{
  int curpos;
  struct DataBlock w;
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
  widbi->w.ptr = NULL;
  widbi->w.size = 0;
  clFreeandclear(dbi);
}

static void dbe_wi_enumfree(struct DataBlockEnumeration *dbe)
{
  struct DBEWindowedEnumeration *widbe = (struct DBEWindowedEnumeration *) dbe->eptr;
  datablockFreePtr(widbe->db);
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
   struct DataBlock *db;
   widbi->w.ptr = widbe->db->ptr + widbi->curpos;
   widbi->w.size = widbe->width;
   db = datablockClonePtr(&widbi->w);
   return db;
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

struct DataBlockEnumeration *dbeLoadWindowed(struct DataBlock *db,
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
  assert(db);
  assert(db->size > 0);
  assert(db->size < 10000000); /* TODO: remove me */
  assert(stepsize > 0);
  assert(width > 0);
  assert(firstpos >= 0);
  assert(lastpos >= firstpos);
  dbe = clCalloc(sizeof(struct DataBlockEnumeration),1);
  *dbe = c;
  dbe->eptr = clCalloc(sizeof(struct DBEWindowedEnumeration), 1);
  widbe = (struct DBEWindowedEnumeration *) dbe->eptr;
  widbe->db = datablockClonePtr(db);
  widbe->firstpos = firstpos;
  widbe->stepsize = stepsize;
  widbe->width = width;
  widbe->lastpos = lastpos;
  return dbe;
}

