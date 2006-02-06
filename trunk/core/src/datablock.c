#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <complearn/complearn.h>

struct DataBlock {
  unsigned char *ptr; /*!< Pointer to data buffer */
	int size;           /*!< Size of data buffer in bytes */
};

/* Allocates memory for a new DataBlock and copies string into new
 * DataBlock without a terminal nul
 */
struct DataBlock *clStringToDataBlockPtr(const char *s)
{
  return clDatablockNewFromBlock(s,strlen(s));
}

struct DataBlock *clFileToDataBlockPtr(const char *path)
{
  struct DataBlock *result;
  FILE *fp = fopen(path, "rb");
  if (fp == NULL) {
    clogError("fopen error reading <%s>", path);
  }
  assert(fp);
  result = clFilePtrToDataBlockPtr(fp);
  fclose(fp);
  return result;
}

struct DataBlock *clFilePtrToDataBlockPtr(FILE *fp)
{
  int toread = 812;
	int incrbytes,totalbytes, i;
  char *dbuf[toread];
  struct DRA *parts = clDraNew();
  unsigned char *ptr;
  unsigned char *partsbuf;
  struct DataBlock *result;

  if (fp == NULL) {
    clogError("NULL ptr in clFilePtrToDataBlockPtr()\n");
  }

  totalbytes = 0;
	while ( (incrbytes = fread(dbuf,1,toread,fp)) > 0) {
    union PCTypes p;
    totalbytes += incrbytes;
    p.dbp = clDatablockNewFromBlock(dbuf,incrbytes);
    clDraPush(parts,p);
  }

  partsbuf = clCalloc(totalbytes,1);
  ptr = partsbuf;

  for ( i = 0; i < clDraSize(parts); i += 1) {
    memcpy(ptr, clDatablockData(clDraGetValueAt(parts,i).dbp), clDatablockSize(clDraGetValueAt(parts,i).dbp));
    ptr += clDatablockSize(clDraGetValueAt(parts,i).dbp);
  }

  clDraFree(parts);

  result = clDatablockNewFromBlock(partsbuf,totalbytes);
  clFree(partsbuf);
  return result;
}

void clDatablockFree(struct DataBlock db)
{
  clFreeandclear(db.ptr);
}

void clDatablockFreePtr(struct DataBlock *db)
{
  clFreeandclear(db->ptr);
  clFreeandclear(db);
}

void clDatablockPrintPtr(struct DataBlock *db)
{
  if (db)
    fwrite(db->ptr, 1, db->size, stdout);
  else
    fwrite("(null)", 1, 6, stdout);
}

char *clDatablockToString(struct DataBlock *db)
{
  char *s;
  if (db == NULL) {
    clogError("NULL ptr in clDatablockToString()\n");
  }
  s = clMalloc(db->size+1);
  memcpy(s, db->ptr, db->size);
	s[db->size] = '\0';
	return s;
}

void clDatablockWriteToFile(struct DataBlock *db, const char *path)
{
	FILE *fp;
	int err;
  if (db == NULL) {
    clogError("NULL ptr in clDatablockWriteToFile()\n");
  }
	fp = clFopen(path,"wb");
  if (fp == NULL) {
    clogError("fopen error in clDatablockWriteToFile()\n");
  }
	err = fwrite(db->ptr,1,db->size,fp);
	if (err == 0) {
		printf("Write error to %s!\n", path);
		exit(1);
	}
  clFclose(fp);
}

struct DataBlock *clDatablockCatPtr(struct DataBlock *a, struct DataBlock *b)
{
	struct DataBlock *d;
  int sz;
  if (a == NULL || b == NULL) {
    clogError("NULL ptr in clDatablockCatPtr()\n");
  }
  sz = a->size + b->size;
  d = clCalloc(sz, 1);
	d->size = sz;
	d->ptr = (unsigned char*)clMalloc(d->size);
  memcpy(d->ptr, a->ptr, a->size);
  memcpy(d->ptr+a->size, b->ptr, b->size);
	return d;
}

struct DataBlock *clDatablockClonePtr(struct DataBlock *db)
{
  if (db == NULL) {
    clogError("NULL ptr in clDatablockCatPtr()\n");
  }
  return clDatablockNewFromBlock(clDatablockData(db),clDatablockSize(db));
}

struct DataBlock *clDatablockNewFromBlock(const void *ptr, unsigned int size)
{
  struct DataBlock *db;
  db = clCalloc(sizeof(struct DataBlock), 1);
  db->size = size;
  db->ptr = clCalloc(size, 1);
  memcpy(db->ptr, ptr, db->size);
  return db;
}

int clDatablockSize(struct DataBlock *db)
{
  if (db == NULL) {
    clogError("NULL ptr in clDatablockSize()\n");
  }
  return db->size;
}

unsigned char *clDatablockData(struct DataBlock *db)
{
  if (db == NULL) {
    clogError("NULL ptr in clDatablockData()\n");
  }
  return db->ptr;
}
