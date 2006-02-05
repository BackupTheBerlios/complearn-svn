#include <string.h>
#include <complearn/complearn.h>

/* TODO: needs new home; does not belong here */
struct DataBlock *clDumpTaggedStringStack(struct StringStack *ss, int tagnum)
{
  struct DataBlock *db, *dblabels;
  db = clStringstackDump(ss);
  dblabels = clPackage_DataBlocks(tagnum,db,NULL);
  clDatablockFreePtr(db);
  return dblabels;
}

/* TODO: needs new home; does not belong here */
struct DataBlock *clCommandsDump(struct StringStack *ss)
{
  return clDumpTaggedStringStack(ss, TAGNUM_COMMANDS);
}

struct DataBlock *clLabelsDump(struct StringStack *ss)
{
  return clDumpTaggedStringStack(ss, TAGNUM_DMLABELS);
}

/* TODO: needs new home; does not belong here */
struct StringStack *clLoadTaggedStringStack(struct DataBlock *db, int fmustbe, const char *tagname, int tagnum)
{
  struct StringStack *ss;
  struct DataBlock *dbss;
  struct DRA *results;
  struct TagHdr *h = (struct TagHdr *) clDatablockData(db);

  if (h->tagnum != tagnum) {
    if (fmustbe) {
      clogError("Error: expecting %s tagnum %x, got %x\n",
          tagname, tagnum,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }
  results = clLoad_DataBlock_package(db);
  dbss = clScanForTag(results, TAGNUM_STRINGSTACK);
  ss = clStringstackLoad(dbss, 1);
  clDatablockFreePtr(dbss);
  clFree_DataBlock_package(results);
  return ss;
}

struct StringStack *clLabelsLoad(struct DataBlock *db, int fmustbe)
{
  return clLoadTaggedStringStack(db, fmustbe, "DMLABELS", TAGNUM_DMLABELS);
}

struct DataBlock *clbLabelsDataBlock(char *fname)
{
  struct DataBlock *db, *dblabels;
  struct DRA *dd;

  db = clFileToDataBlockPtr(fname);
  dd = clLoad_DataBlock_package(db);
  dblabels = clScanForTag(dd, TAGNUM_DMLABELS);

  clDatablockFreePtr(db);
  clDraFree(dd);

  return dblabels;
}

struct StringStack *clbLabelsLoad(struct DataBlock *db)
{
  struct StringStack *labels;
  labels = clLabelsLoad(db, 1);
  return labels;
}

struct StringStack *clbDBLabels(struct DataBlock *db)
{
  struct DataBlock *dblabels;
  struct DRA *dd;
  struct StringStack *ss;

  dd = clLoad_DataBlock_package(db);
  dblabels = clScanForTag(dd, TAGNUM_DMLABELS);

  ss = clbLabelsLoad(dblabels);

  clDatablockFreePtr(dblabels);
  clDraFree(dd);
  return ss;
}

struct StringStack *clbLabels(char *fname)
{
  struct DataBlock *db;
  struct StringStack *result;

  db = clbLabelsDataBlock(fname);
  result = clbLabelsLoad(db);

  clDatablockFreePtr(db);
  return result;
}

#define DELIMS " ,\t\r\n"
#define MAXLINESIZE 1024
struct StringStack *cltxtLabels(char *fname)
{
  struct StringStack *labels = NULL;
  FILE *fp;
  char linebuf[MAXLINESIZE];

  if ( cltxtRowSize(fname) == cltxtColSize(fname) ) {
    clogError("Error: no labels in this file\n");
    exit(1);
  }
  labels = clStringstackNew();
  fp = clFopen(fname, "r");
  while (fgets(linebuf, MAXLINESIZE, fp)) {
    clStringstackPush(labels, strtok(linebuf,DELIMS));
  }
  return labels;
}

/* TODO: needs new home; does not belong here */
struct StringStack *clbCommands(char *fname)
{
  struct DataBlock *db, *dbem;
  struct DRA *dd;
  struct StringStack *result;

  db = clFileToDataBlockPtr(fname);
  dd = clLoad_DataBlock_package(db);
  dbem = clScanForTag(dd, TAGNUM_COMMANDS);
  result = clLoadTaggedStringStack(dbem, 1, "COMMANDS", TAGNUM_COMMANDS);

  clDatablockFreePtr(db);
  clDatablockFreePtr(dbem);
  clDraFree(dd);
  return result;
}
