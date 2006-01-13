#include <string.h>
#include <complearn/complearn.h>

/* TODO: needs new home; does not belong here */
struct DataBlock *dumpTaggedStringStack(struct StringStack *ss, int tagnum)
{
  struct DataBlock *db, *dblabels;
  db = stringstackDump(ss);
  dblabels = package_DataBlocks(tagnum,db,NULL);
  datablockFreePtr(db);
  return dblabels;
}

/* TODO: needs new home; does not belong here */
struct DataBlock *commandsDump(struct StringStack *ss)
{
  return dumpTaggedStringStack(ss, TAGNUM_COMMANDS);
}

struct DataBlock *labelsDump(struct StringStack *ss)
{
  return dumpTaggedStringStack(ss, TAGNUM_DMLABELS);
}

/* TODO: needs new home; does not belong here */
struct StringStack *loadTaggedStringStack(struct DataBlock *db, int fmustbe, const char *tagname, int tagnum)
{
  struct StringStack *ss;
  struct DataBlock *dbss;
  struct DoubleA *results;
  struct TagHdr *h = (struct TagHdr *) datablockData(db);

  if (h->tagnum != tagnum) {
    if (fmustbe) {
      clogError("Error: expecting %s tagnum %x, got %x\n",
          tagname, tagnum,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }
  results = load_DataBlock_package(db);
  dbss = scanForTag(results, TAGNUM_STRINGSTACK);
  ss = stringstackLoad(dbss, 1);
  datablockFreePtr(dbss);
  doubleaFree(results);
  return ss;
}

struct StringStack *labelsLoad(struct DataBlock *db, int fmustbe)
{
  return loadTaggedStringStack(db, fmustbe, "DMLABELS", TAGNUM_DMLABELS);
}
struct DataBlock *clbLabelsDataBlock(char *fname)
{
  struct DataBlock *db, *dblabels;
  struct DoubleA *dd;

  db = fileToDataBlockPtr(fname);
  dd = load_DataBlock_package(db);
  dblabels = scanForTag(dd, TAGNUM_DMLABELS);

  datablockFreePtr(db);
  doubleaFree(dd);

  return dblabels;
}

struct StringStack *clbLabelsLoad(struct DataBlock *db)
{
  struct StringStack *labels;
  labels = labelsLoad(db, 1);
  return labels;
}

struct StringStack *clbLabels(char *fname)
{
  struct DataBlock *db;
  struct StringStack *result;

  db = clbLabelsDataBlock(fname);
  result = clbLabelsLoad(db);

  datablockFreePtr(db);
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
  labels = stringstackNew();
  fp = clfopen(fname, "r");
  while (fgets(linebuf, MAXLINESIZE, fp)) {
    stringstackPush(labels, strtok(linebuf,DELIMS));
  }
  return labels;
}

/* TODO: needs new home; does not belong here */
struct StringStack *clbCommands(char *fname)
{
  struct DataBlock *db, *dbem;
  struct DoubleA *dd;
  struct StringStack *result;

  db = fileToDataBlockPtr(fname);
  dd = load_DataBlock_package(db);
  dbem = scanForTag(dd, TAGNUM_COMMANDS);
  result = loadTaggedStringStack(dbem, 1, "COMMANDS", TAGNUM_COMMANDS);

  datablockFreePtr(db);
  datablockFreePtr(dbem);
  doubleaFree(dd);
  return result;
}
