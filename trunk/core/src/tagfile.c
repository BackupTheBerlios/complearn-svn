#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <complearn/complearn.h>

struct TagManager {
  struct DataBlock *db;
  unsigned char *cur;
  int read;
  int size;
};

struct TagManager *newTagManager(struct DataBlock *db)
{
  struct TagManager *tm = clCalloc(sizeof(*tm),1);
  struct TagHdr *h = (struct TagHdr *) db->ptr;
  tm->db = db;
  tm->cur = db->ptr + sizeof(struct TagHdr);
  tm->size = h->size;
  tm->read = 0;
  return tm;
}

t_tagtype getCurTagNum(const struct TagManager *tm)
{
  struct TagHdr *h = (struct TagHdr *) tm->cur;
  return h->tagnum;
}

void stepNextDataBlock(struct TagManager *tm)
{
  struct TagHdr *h = (struct TagHdr *) tm->cur;
  tm->cur += h->size + sizeof(*h);
  tm->read += h->size + sizeof(*h);
}

int getCurDataBlock(struct TagManager *tm, struct DataBlock *cur)
{
  struct TagHdr *h = (struct TagHdr *) tm->cur;
  if (tm->read >= tm->size) return 0;
  cur->ptr = tm->cur;
  cur->size = h->size + sizeof(*h);
  return 1;
}

void freeTagManager(struct TagManager *tm)
{
  tm->cur = NULL;
  tm->db = NULL;
  clFreeandclear(tm);
}

struct DataBlock *package_DataBlocks(t_tagtype overalltag, ...)
{
  va_list ap;
  struct DataBlock *db, *result;
  struct DoubleA *parts = doubleaNew();
  va_start(ap, overalltag);
  while ( (db = va_arg(ap, struct DataBlock *)) ) {
    union PCTypes p = zeropct;
    p.db = *db;
    doubleaPush(parts, p);
  }
  va_end(ap);

  result = package_dd_DataBlocks(overalltag,parts);

  doubleaFree(parts);
  return result;
}

struct DataBlock *package_dd_DataBlocks(t_tagtype tnum, struct DoubleA *parts)
{
  struct DataBlock result,cur;
  struct TagHdr h;
  int i;
  unsigned char *ptr;

  cur.size = 0;

  for ( i = 0; i < doubleaSize(parts); i += 1) {
    cur.size += doubleaGetValueAt(parts,i).db.size;
  }
  cur.ptr = clCalloc(cur.size,1);
  ptr = cur.ptr;

  for ( i = 0; i < doubleaSize(parts); i += 1) {
    memcpy(ptr, doubleaGetValueAt(parts,i).db.ptr, doubleaGetValueAt(parts,i).db.size);
    ptr += doubleaGetValueAt(parts,i).db.size;
  }

  h.tagnum = tnum;
  h.size = cur.size;

  result.size = sizeof(h) + cur.size;
  result.ptr = clCalloc(result.size,1);
  memcpy(result.ptr, &h, sizeof(h));
  memcpy(result.ptr + sizeof(h), cur.ptr, h.size);
  clFree(cur.ptr);
  {
    struct DataBlock *rr = clCalloc(sizeof(struct DataBlock), 1);
    *rr = result;
    return rr;
  }
}

void free_DataBlock_package ( struct DoubleA *da, void *udata)
{
  int i;
  for ( i = 0; i < doubleaSize(da) ; i += 1) {
    datablockFreePtr(doubleaGetValueAt(da,i).idbp.db);
  }
}

struct DoubleA *load_DataBlock_package(struct DataBlock *db)
{
  struct DoubleA *result = doubleaNew();
  struct TagManager *tm;
  struct DataBlock cur;

  tm = newTagManager(db);

  while (getCurDataBlock(tm, &cur)) {
    union PCTypes p = zeropct;
    p.idbp.tnum = getCurTagNum(tm);
    p.idbp.db = datablockClonePtr(&cur);
    doubleaPush(result, p);
    stepNextDataBlock(tm);
  }
  freeTagManager(tm);

  return result;
}

struct DataBlock *scanForTag(struct DoubleA *dd, int tnum)
{
  int i;
  struct DataBlock db;
  t_tagtype curtnum;
  for (i = 0; i < doubleaSize(dd); i += 1) {
    curtnum = doubleaGetValueAt(dd,i).idbp.tnum;
    if (curtnum == tnum) {
      db = *doubleaGetValueAt(dd,i).idbp.db;
      return datablockClonePtr(&db);
    }
  }
  return NULL;
}
