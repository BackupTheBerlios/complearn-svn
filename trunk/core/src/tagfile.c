#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <complearn/complearn.h>

struct TagManager {
  struct DataBlock db;
  unsigned char *cur;
  int read;
  int size;
};

struct TagManager *newTagManager(struct DataBlock db)
{
  struct TagManager *tm = gcalloc(sizeof(*tm),1);
  struct tagHdr *h = (struct tagHdr *) db.ptr;
  tm->db = db;
  tm->cur = db.ptr + sizeof(struct tagHdr);
  tm->size = h->size;
  tm->read = 0;
  return tm;
}

t_tagtype getCurTagNum(const struct TagManager *tm)
{
  struct tagHdr *h = (struct tagHdr *) tm->cur;
  return h->tagnum;
}

int getCurTagHdrSize(const struct TagManager *tm)
{
  struct tagHdr *h = (struct tagHdr *) tm->cur;
  return h->size;
}

void stepNextDataBlock(struct TagManager *tm)
{
  struct tagHdr *h = (struct tagHdr *) tm->cur;
  tm->cur += h->size + sizeof(*h);
  tm->read += h->size + sizeof(*h);
}

int getCurDataBlock(struct TagManager *tm, struct DataBlock *cur)
{
  struct tagHdr *h = (struct tagHdr *) tm->cur;
  if (tm->read >= tm->size) return 0;
  cur->ptr = tm->cur;
  cur->size = h->size + sizeof(*h);
  return 1;
}

void freeTagManager(struct TagManager *tm)
{
  tm->cur = NULL;
  tm->db.ptr = NULL;
  gfreeandclear(tm);
}

struct DataBlock package_DataBlocks(t_tagtype overalltag, ...)
{
  va_list ap;
  struct DataBlock *db, result;
  struct DoubleA *parts = newDoubleDoubler();
  va_start(ap, overalltag);
  while ( (db = va_arg(ap, struct DataBlock *)) ) {
    union pctypes p = zeropct;
    p.db = *db;
    pushValue(parts, p);
  }
  va_end(ap);

  result = package_dd_DataBlocks(overalltag,parts);

  freeDoubleDoubler(parts);
  return result;
}

struct DataBlock package_dd_DataBlocks(t_tagtype tnum, struct DoubleA *parts)
{
  struct DataBlock result,cur;
  struct tagHdr h;
  int i;
  unsigned char *ptr;

  cur.size = 0;

  for ( i = 0; i < getSize(parts); i += 1) {
    cur.size += getValueAt(parts,i).db.size;
  }
  cur.ptr = gcalloc(cur.size,1);
  ptr = cur.ptr;

  for ( i = 0; i < getSize(parts); i += 1) {
    memcpy(ptr, getValueAt(parts,i).db.ptr, getValueAt(parts,i).db.size);
    ptr += getValueAt(parts,i).db.size;
  }

  h.tagnum = tnum;
  h.size = cur.size;

  result.size = sizeof(h) + cur.size;
  result.ptr = gcalloc(result.size,1);
  memcpy(result.ptr, &h, sizeof(h));
  memcpy(result.ptr + sizeof(h), cur.ptr, h.size);
  freeDataBlock(cur);
  return result;
}

void free_DataBlock_package ( struct DoubleA *da, void *udata)
{
  int i;
  for ( i = 0; i < getSize(da) ; i += 1) {
    freeDataBlock(getValueAt(da,i).idbp.db);
  }
}

struct DoubleA *load_DataBlock_package(struct DataBlock db)
{
  struct DoubleA *result = newDoubleDoubler();
  struct TagManager *tm;
  struct DataBlock cur;

  tm = newTagManager(db);

  while (getCurDataBlock(tm, &cur)) {
    union pctypes p = zeropct;
    p.idbp.tnum = getCurTagNum(tm);
    p.idbp.db = cur;
    pushValue(result, p);
    stepNextDataBlock(tm);
  }
  freeTagManager(tm);

  return result;
}

struct DataBlock scanForTag(struct DoubleA *dd, int tnum)
{
  int i;
  struct DataBlock db, dbclone;
  t_tagtype curtnum;
  for (i = 0; i < getSize(dd); i += 1) {
    curtnum = getValueAt(dd,i).idbp.tnum;
    if (curtnum == tnum) {
      db = getValueAt(dd,i).idbp.db;
      dbclone =  cloneDataBlock(db);
      return dbclone;
    }
  }
  return db;
}
