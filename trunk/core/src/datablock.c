#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "clmalloc.h"
#include <complearn/complearn.h>

struct DataBlock {
  unsigned char *ptr; /*!< Pointer to data buffer */
	int size;           /*!< Size of data buffer in bytes */
};

/* Allocates memory for a new DataBlock and copies string into new
 * DataBlock without a terminal nul
 */
struct DataBlock *stringToDataBlockPtr(const char *s)
{
  return datablockNewFromBlock(s,strlen(s));
}

struct DataBlock *fileToDataBlockPtr(const char *path)
{
  struct DataBlock *result;
  FILE *fp = fopen(path, "rb");
  if (fp == NULL) {
    clogError("fopen error reading <%s>", path);
  }
  assert(fp);
  result = filePtrToDataBlockPtr(fp);
  fclose(fp);
  return result;
}

struct DataBlock *filePtrToDataBlockPtr(FILE *fp)
{
  int toread = 812;
	int incrbytes,totalbytes, i;
  char *dbuf[toread];
  struct DoubleA *parts = doubleaNew();
  unsigned char *ptr;
  unsigned char *partsbuf;
  struct DataBlock *result;

  if (fp == NULL) {
    clogError("NULL ptr in filePtrToDataBlockPtr()\n");
  }

  totalbytes = 0;
	while ( (incrbytes = fread(dbuf,1,toread,fp)) > 0) {
    union PCTypes p;
    totalbytes += incrbytes;
    p.dbp = datablockNewFromBlock(dbuf,incrbytes);
    doubleaPush(parts,p);
  }

  partsbuf = clCalloc(totalbytes,1);
  ptr = partsbuf;

  for ( i = 0; i < doubleaSize(parts); i += 1) {
    memcpy(ptr, datablockData(doubleaGetValueAt(parts,i).dbp), datablockSize(doubleaGetValueAt(parts,i).dbp));
    ptr += datablockSize(doubleaGetValueAt(parts,i).dbp);
  }

  doubleaFree(parts);

  result = datablockNewFromBlock(partsbuf,totalbytes);
  clFree(partsbuf);
  return result;
}

void datablockFree(struct DataBlock db)
{
  clFreeandclear(db.ptr);
}

void datablockFreePtr(struct DataBlock *db)
{
  clFreeandclear(db->ptr);
  clFreeandclear(db);
}

void datablockPrintPtr(struct DataBlock *db)
{
  if (db)
    fwrite(db->ptr, 1, db->size, stdout);
  else
    fwrite("(null)", 1, 6, stdout);
}

char *datablockToString(struct DataBlock *db)
{
  char *s;
  if (db == NULL) {
    clogError("NULL ptr in datablockToString()\n");
  }
  s = clMalloc(db->size+1);
  memcpy(s, db->ptr, db->size);
	s[db->size] = '\0';
	return s;
}

void datablockWriteToFile(struct DataBlock *db, const char *path)
{
	FILE *fp;
	int err;
  if (db == NULL) {
    clogError("NULL ptr in datablockWriteToFile()\n");
  }
	fp = clFopen(path,"wb");
	err = fwrite(db->ptr,1,db->size,fp);
	if (err == 0) {
		printf("Write error to %s!\n", path);
		exit(1);
	}
  clFclose(fp);
}

struct DataBlock *datablockCatPtr(struct DataBlock *a, struct DataBlock *b)
{
	struct DataBlock *d;
  int sz;
  if (a == NULL || b == NULL) {
    clogError("NULL ptr in datablockCatPtr()\n");
  }
  sz = a->size + b->size;
  d = clCalloc(sz, 1);
	d->size = sz;
	d->ptr = (unsigned char*)clMalloc(d->size);
  memcpy(d->ptr, a->ptr, a->size);
  memcpy(d->ptr+a->size, b->ptr, b->size);
	return d;
}

struct DataBlock *datablockClonePtr(struct DataBlock *db)
{
  if (db == NULL) {
    clogError("NULL ptr in datablockCatPtr()\n");
  }
  return datablockNewFromBlock(datablockData(db),datablockSize(db));
}

struct DataBlock *datablockNewFromBlock(const void *ptr, unsigned int size)
{
  struct DataBlock *db;
  db = clCalloc(sizeof(struct DataBlock), 1);
  db->size = size;
  db->ptr = clCalloc(size, 1);
  memcpy(db->ptr, ptr, db->size);
  return db;
}

int datablockSize(struct DataBlock *db)
{
  if (db == NULL) {
    clogError("NULL ptr in datablockSize()\n");
  }
  return db->size;
}

unsigned char *datablockData(struct DataBlock *db)
{
  if (db == NULL) {
    clogError("NULL ptr in datablockData()\n");
  }
  return db->ptr;
}
