#include <stdarg.h>
#include <string.h>
#include "clalloc.h"
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
  struct TagHdr *h = (struct TagHdr *) datablockData(db);
  tm->db = db;
  tm->cur = datablockData(db) + sizeof(struct TagHdr);
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

struct DataBlock *getCurDataBlock(struct TagManager *tm)
{
  struct TagHdr *h = (struct TagHdr *) tm->cur;
  if (tm->read >= tm->size) return NULL;
  return datablockNewFromBlock(tm->cur, h->size + sizeof(*h));
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
  struct DRA *parts = draNew();
  va_start(ap, overalltag);
  while ( (db = va_arg(ap, struct DataBlock *)) ) {
    union PCTypes p = zeropct;
    p.dbp = db;
    draPush(parts, p);
  }
  va_end(ap);

  result = package_dd_DataBlocks(overalltag,parts);

  draFree(parts);
  return result;
}

struct DataBlock *package_dd_DataBlocks(t_tagtype tnum, struct DRA *parts)
{
  struct DataBlock *result;
  struct TagHdr h;
  int i, ressize;
  unsigned char *ptr;
  int dbsize = 0;
  unsigned char *dbptr, *resptr;

  for ( i = 0; i < draSize(parts); i += 1) {
    dbsize += datablockSize(draGetValueAt(parts,i).dbp);
  }
  dbptr = clCalloc(dbsize,1);
  ptr = dbptr;

  for ( i = 0; i < draSize(parts); i += 1) {
    memcpy(ptr, datablockData(draGetValueAt(parts,i).dbp), datablockSize(draGetValueAt(parts,i).dbp));
    ptr += datablockSize(draGetValueAt(parts,i).dbp);
  }

  h.tagnum = tnum;
  h.size = dbsize;

  ressize = sizeof(h) + dbsize;
  resptr = clCalloc(ressize,1);
  memcpy(resptr, &h, sizeof(h));
  memcpy(resptr + sizeof(h), dbptr, h.size);

  clFree(dbptr);

  result =  datablockNewFromBlock(resptr,ressize);
  clFree(resptr);
  return result;

}

void free_DataBlock_package ( struct DRA *da, void *udata)
{
  int i;
  for ( i = 0; i < draSize(da) ; i += 1) {
    datablockFreePtr(draGetValueAt(da,i).idbp.db);
  }
}

struct DRA *load_DataBlock_package(struct DataBlock *db)
{
  struct DRA *result = draNew();
  struct TagManager *tm;
  struct DataBlock *cur = NULL;

  tm = newTagManager(db);

  while ((cur = getCurDataBlock(tm))) {
    union PCTypes p = zeropct;
    p.idbp.tnum = getCurTagNum(tm);
    p.idbp.db = datablockClonePtr(cur);
    draPush(result, p);
    stepNextDataBlock(tm);
    datablockFreePtr(cur);
  }
  freeTagManager(tm);

  return result;
}

struct DataBlock *scanForTag(struct DRA *dd, int tnum)
{
  int i;
  struct DataBlock *db;
  t_tagtype curtnum;
  for (i = 0; i < draSize(dd); i += 1) {
    curtnum = draGetValueAt(dd,i).idbp.tnum;
    if (curtnum == tnum) {
      db = draGetValueAt(dd,i).idbp.db;
      return datablockClonePtr(db);
    }
  }
  return NULL;
}
