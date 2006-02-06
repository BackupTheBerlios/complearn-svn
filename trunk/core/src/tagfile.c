#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <complearn/complearn.h>

struct TagManager {
  struct DataBlock *db;
  unsigned char *cur;
  int read;
  int size;
};

struct TagManager *clNewTagManager(struct DataBlock *db)
{
  struct TagManager *tm = clCalloc(sizeof(*tm),1);
  struct TagHdr *h = (struct TagHdr *) clDatablockData(db);
  tm->db = db;
  tm->cur = clDatablockData(db) + sizeof(struct TagHdr);
  tm->size = h->size;
  tm->read = 0;
  return tm;
}

t_tagtype clGetCurTagNum(const struct TagManager *tm)
{
  struct TagHdr *h = (struct TagHdr *) tm->cur;
  return h->tagnum;
}

void clStepNextDataBlock(struct TagManager *tm)
{
  struct TagHdr *h = (struct TagHdr *) tm->cur;
  tm->cur += h->size + sizeof(*h);
  tm->read += h->size + sizeof(*h);
}

struct DataBlock *clGetCurDataBlock(struct TagManager *tm)
{
  struct TagHdr *h = (struct TagHdr *) tm->cur;
  if (tm->read >= tm->size) return NULL;
  return clDatablockNewFromBlock(tm->cur, h->size + sizeof(*h));
}

void clFreeTagManager(struct TagManager *tm)
{
  tm->cur = NULL;
  tm->db = NULL;
  clFreeandclear(tm);
}

struct DataBlock *clPackage_DataBlocks(t_tagtype overalltag, ...)
{
  va_list ap;
  struct DataBlock *db, *result;
  struct DRA *parts = clDraNew();
  va_start(ap, overalltag);
  while ( (db = va_arg(ap, struct DataBlock *)) ) {
    union PCTypes p = zeropct;
    p.dbp = db;
    clDraPush(parts, p);
  }
  va_end(ap);

  result = clPackage_dd_DataBlocks(overalltag,parts);

  clDraFree(parts);
  return result;
}

struct DataBlock *clPackage_dd_DataBlocks(t_tagtype tnum, struct DRA *parts)
{
  struct DataBlock *result;
  struct TagHdr h;
  int i, ressize;
  unsigned char *ptr;
  int dbsize = 0;
  unsigned char *dbptr, *resptr;

  for ( i = 0; i < clDraSize(parts); i += 1) {
    dbsize += clDatablockSize(clDraGetValueAt(parts,i).dbp);
  }
  dbptr = clCalloc(dbsize,1);
  ptr = dbptr;

  for ( i = 0; i < clDraSize(parts); i += 1) {
    memcpy(ptr, clDatablockData(clDraGetValueAt(parts,i).dbp), clDatablockSize(clDraGetValueAt(parts,i).dbp));
    ptr += clDatablockSize(clDraGetValueAt(parts,i).dbp);
  }

  h.tagnum = tnum;
  h.size = dbsize;

  ressize = sizeof(h) + dbsize;
  resptr = clCalloc(ressize,1);
  memcpy(resptr, &h, sizeof(h));
  memcpy(resptr + sizeof(h), dbptr, h.size);

  clFree(dbptr);

  result =  clDatablockNewFromBlock(resptr,ressize);
  clFree(resptr);
  return result;

}

void clFree_DataBlock_package ( struct DRA *da )
{
  int i;
  for ( i = 0; i < clDraSize(da) ; i += 1) {
    clDatablockFreePtr(clDraGetValueAt(da,i).idbp.db);
  }
  clDraFree(da);
}

struct DRA *clLoad_DataBlock_package(struct DataBlock *db)
{
  struct DRA *result = clDraNew();
  struct TagManager *tm;
  struct DataBlock *cur = NULL;

  tm = clNewTagManager(db);

  while ((cur = clGetCurDataBlock(tm))) {
    union PCTypes p = zeropct;
    p.idbp.tnum = clGetCurTagNum(tm);
    p.idbp.db = clDatablockClonePtr(cur);
    clDraPush(result, p);
    clStepNextDataBlock(tm);
    clDatablockFreePtr(cur);
  }
  clFreeTagManager(tm);

  return result;
}

struct DataBlock *clScanForTag(struct DRA *dd, int tnum)
{
  int i;
  struct DataBlock *db;
  t_tagtype curtnum;
  for (i = 0; i < clDraSize(dd); i += 1) {
    curtnum = clDraGetValueAt(dd,i).idbp.tnum;
    if (curtnum == tnum) {
      db = clDraGetValueAt(dd,i).idbp.db;
      return clDatablockClonePtr(db);
    }
  }
  return NULL;
}
