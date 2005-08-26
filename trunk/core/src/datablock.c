#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <complearn/complearn.h>

/* Allocates memory for a new DataBlock and copies string into new
 * DataBlock without a terminal nul
 */
struct DataBlock convertStringToDataBlock(const char *s) {
  struct DataBlock d;
  d.size = strlen(s);
  d.ptr = gmalloc(d.size);
  memcpy(d.ptr, s, d.size);
	return d;
}

struct DataBlock convertFileToDataBlock(const char *path)
{
  struct DataBlock result;
  FILE *fp = fopen(path, "rb");
  if (fp == NULL) {
    printf("Converting file to DataBlock: error opening <%s>\n", path);
    perror("fopen");
    exit(1);
  }
  assert(fp);
  result = convertFileToDataBlockFP(fp);
  fclose(fp);
  return result;
}

struct DataBlock convertFileToDataBlockFP(FILE *fp)
{
  struct DataBlock d;
  int toread = 812;
	int bytesread, i;
  char *dbuf[toread];
  struct DoubleA *parts = newDoubleDoubler();
  unsigned char *ptr;

  d.size = 0;
	while ( (bytesread = fread(dbuf,1,toread,fp)) > 0) {
    union PCTypes p;
    p.db.size = bytesread;
    d.size += bytesread;
    p.db.ptr = (unsigned char*)gmalloc(p.db.size);
    memcpy(p.db.ptr, dbuf, p.db.size);
    pushValue(parts,p);
  }

  d.ptr = gcalloc(d.size,1);
  ptr = d.ptr;

  for ( i = 0; i < getSize(parts); i += 1) {
    memcpy(ptr, getValueAt(parts,i).db.ptr, getValueAt(parts,i).db.size);
    ptr += getValueAt(parts,i).db.size;
  }

  freeDoubleDoubler(parts);

  return d;
}

void freeDataBlock(struct DataBlock db)
{
  gfreeandclear(db.ptr);
}

void freeDataBlockPtr(struct DataBlock *db)
{
  gfreeandclear(db->ptr);
  gfreeandclear(db);
}

void printDataBlockPtr(struct DataBlock *db)
{
  if (db)
    printDataBlock(*db);
  else
    fwrite("(null)", 1, 6, stdout);
}

void printDataBlock(struct DataBlock db)
{
  fwrite(db.ptr, 1, db.size, stdout);
}

char *convertDataBlockToString(struct DataBlock db)
{
  char *s;
  s = gmalloc(db.size+1);
  memcpy(s, db.ptr, db.size);
	s[db.size] = '\0';
	return s;
}

void writeDataBlockToFile(struct DataBlock *db, const char *path)
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

struct DataBlock catDataBlock (struct DataBlock a, struct DataBlock b)
{
	struct DataBlock d;
	d.size = a.size + b.size;
	d.ptr = (unsigned char*)gmalloc(d.size);
  memcpy(d.ptr, a.ptr, a.size);
  memcpy(d.ptr+a.size, b.ptr, b.size);
	return d;
}

struct DataBlock cloneDataBlock(struct DataBlock db)
{
  struct DataBlock result;
  result.size = db.size;
  result.ptr = gmalloc(db.size);
  memcpy(result.ptr, db.ptr, db.size);
  return result;
}

struct DataBlock *cloneDataBlockPtr(struct DataBlock *ptr)
{
  struct DataBlock *result;
  result = gcalloc(sizeof(*result), 1);
  *result = cloneDataBlock(*ptr);
  return result;
}
