#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include <complearn/complearn.h>

/** \brief Private enumeration instance for an array enumeration.
 *  \struct DBEArrayEnumeration
 */
struct DBEArrayEnumeration
{
  struct DataBlock *db;
  int size;
};

/** \brief Iterator for a DBEArrayEnumeration.
 *  \struct DBEArrayEnumerationIterator
 */
struct DBEArrayEnumerationIterator
{
  int cur;
  char curlabel[32];
};

static struct DataBlockEnumerationIterator *dbe_ar_newenumiter(struct DataBlockEnumeration *ptr)
{
  return gcalloc(sizeof(struct DBEArrayEnumerationIterator), 1);
}

static void dbe_ar_iterfree(struct DataBlockEnumerationIterator *dbi)
{
  gfreeandclear(dbi);
}
static void dbe_ar_enumfree(struct DataBlockEnumeration *dbe)
{
  struct DBEArrayEnumeration *ardbe = (struct DBEArrayEnumeration *) dbe->eptr;
  gfreeandclear(ardbe->db);
  gfreeandclear(dbe->eptr);
  gfreeandclear(dbe);
}
static struct DataBlock *dbe_ar_istar(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEArrayEnumeration *ardbe = (struct DBEArrayEnumeration *) dbe->eptr;
  struct DBEArrayEnumerationIterator *ardbi = (struct DBEArrayEnumerationIterator *) dbi;
  struct DataBlock *old, *cur = NULL;
  old = (ardbi->cur >= 0 && ardbi->cur < ardbe->size) ? ardbe->db + ardbi->cur : NULL;
  if (old) {
    cur = gcalloc(sizeof(struct DataBlock), 1);
    *cur = datablockClone(*old);
  }
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

struct DataBlockEnumeration *loadArrayDBE(struct DataBlock *db, int size)
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
  dbe = gcalloc(sizeof(struct DataBlockEnumeration),1);
  *dbe = e;
  dbe->eptr = gcalloc(sizeof(struct DBEArrayEnumeration), 1);
  ardbe = (struct DBEArrayEnumeration *) dbe->eptr;
  ardbe->db = gcalloc(sizeof(struct DataBlock), size);
  memcpy(ardbe->db, db, sizeof(struct DataBlock) * size);
  ardbe->size = size;
  return dbe;
}
