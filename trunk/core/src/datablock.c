#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <complearn/complearn.h>

/* Allocates memory for a new DataBlock and copies string into new
 * DataBlock without a terminal nul
 */
struct DataBlock *stringToDataBlockPtr(const char *s) {
  struct DataBlock *d = clCalloc(sizeof(struct DataBlock), 1);
  d->size = strlen(s);
  d->ptr = clMalloc(d->size);
  memcpy(d->ptr, s, d->size);
	return d;
}

struct DataBlock *fileToDataBlockPtr(const char *path)
{
  struct DataBlock *result;
  FILE *fp = fopen(path, "rb");
  if (fp == NULL) {
    printf("Converting file to DataBlock: error opening <%s>\n", path);
    perror("fopen");
    exit(1);
  }
  assert(fp);
  result = filePtrToDataBlockPtr(fp);
  fclose(fp);
  return result;
}

struct DataBlock *filePtrToDataBlockPtr(FILE *fp)
{
  struct DataBlock *d;
  d = clCalloc(sizeof(struct DataBlock), 1);
  int toread = 812;
	int bytesread, i;
  char *dbuf[toread];
  struct DoubleA *parts = doubleaNew();
  unsigned char *ptr;

  d->size = 0;
	while ( (bytesread = fread(dbuf,1,toread,fp)) > 0) {
    union PCTypes p;
    p.db.size = bytesread;
    d->size += bytesread;
    p.db.ptr = (unsigned char*)clMalloc(p.db.size);
    memcpy(p.db.ptr, dbuf, p.db.size);
    doubleaPush(parts,p);
  }

  d->ptr = clCalloc(d->size,1);
  ptr = d->ptr;

  for ( i = 0; i < doubleaSize(parts); i += 1) {
    memcpy(ptr, doubleaGetValueAt(parts,i).db.ptr, doubleaGetValueAt(parts,i).db.size);
    ptr += doubleaGetValueAt(parts,i).db.size;
  }

  doubleaFree(parts);

  return d;
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
    datablockPrint(*db);
  else
    fwrite("(null)", 1, 6, stdout);
}

void datablockPrint(struct DataBlock db)
{
  fwrite(db.ptr, 1, db.size, stdout);
}

char *datablockToString(struct DataBlock *db)
{
  char *s;
  s = clMalloc(db->size+1);
  memcpy(s, db->ptr, db->size);
	s[db->size] = '\0';
	return s;
}

void datablockWriteToFile(struct DataBlock *db, const char *path)
{
	FILE *fp;
	int err;
	fp = clfopen(path,"wb");
	err = fwrite(db->ptr,1,db->size,fp);
	if (err == 0) {
		printf("Write error to %s!\n", path);
		exit(1);
	}
  clfclose(fp);
}

struct DataBlock *datablockCatPtr(struct DataBlock *a, struct DataBlock *b)
{
	struct DataBlock *d;
  int sz = a->size + b->size;
  d = clCalloc(sz, 1);
	d->size = sz;
	d->ptr = (unsigned char*)clMalloc(d->size);
  memcpy(d->ptr, a->ptr, a->size);
  memcpy(d->ptr+a->size, b->ptr, b->size);
	return d;
}

struct DataBlock datablockClone(struct DataBlock db)
{
  struct DataBlock result;
  result.size = db.size;
  result.ptr = clMalloc(db.size);
  memcpy(result.ptr, db.ptr, db.size);
  return result;
}

struct DataBlock *datablockClonePtr(struct DataBlock *ptr)
{
  struct DataBlock *result;
  result = clCalloc(sizeof(*result), 1);
  *result = datablockClone(*ptr);
  return result;
}
