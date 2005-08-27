#include <string.h>
#include <complearn/complearn.h>

/* TODO: needs new home; does not belong here */
struct DataBlock dumpTaggedStringStack(struct StringStack *ss, int tagnum)
{
  struct DataBlock db, dblabels;
  db = stringDumpStack(ss);
  dblabels = package_DataBlocks(tagnum,&db,NULL);
  datablockFree(db);
  return dblabels;
}

/* TODO: needs new home; does not belong here */
struct DataBlock dumpCommands(struct StringStack *ss)
{
  return dumpTaggedStringStack(ss, TAGNUM_COMMANDS);
}

struct DataBlock dumpDMLabels(struct StringStack *ss)
{
  return dumpTaggedStringStack(ss, TAGNUM_DMLABELS);
}

/* TODO: needs new home; does not belong here */
struct StringStack *loadTaggedStringStack(struct DataBlock db, int fmustbe, const char *tagname, int tagnum)
{
  struct StringStack *ss;
  struct DataBlock dbss;
  struct DoubleA *results;
  struct TagHdr *h = (struct TagHdr *) db.ptr;

  if (h->tagnum != tagnum) {
    if (fmustbe) {
      fprintf(stderr,"Error: expecting %s tagnum %x, got %x\n",
          tagname, tagnum,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }
  results = load_DataBlock_package(db);
  dbss = scanForTag(results, TAGNUM_STRINGSTACK);
  ss = stringLoadStack(dbss, 1);
  datablockFree(dbss);
  doubleaFree(results);
  return ss;
}

struct StringStack *loadDMLabels(struct DataBlock db, int fmustbe)
{
  return loadTaggedStringStack(db, fmustbe, "DMLABELS", TAGNUM_DMLABELS);
}

struct StringStack *get_labels_from_clb(char *fname)
{
  struct DataBlock db, dblabels;
  struct DoubleA *dd;
  struct StringStack *result;

  db = fileToDataBlock(fname);
  dd = load_DataBlock_package(db);
  dblabels = scanForTag(dd, TAGNUM_DMLABELS);
  result = loadDMLabels(dblabels, 1);

  datablockFree(db);
  datablockFree(dblabels);
  doubleaFree(dd);

  return result;
}

#define DELIMS " ,\t\r\n"
#define MAXLINESIZE 1024
struct StringStack *get_labels_from_txt(char *fname)
{
  struct StringStack *labels = NULL;
  FILE *fp;
  char linebuf[MAXLINESIZE];

  if ( get_row_size_from_txt(fname) == get_col_size_from_txt(fname) ) {
    fprintf(stderr,"Error: no labels in this file\n");
    exit(1);
  }
  labels = newStringStack();
  fp = clfopen(fname, "r");
  while (fgets(linebuf, MAXLINESIZE, fp)) {
    pushSS(labels, strtok(linebuf,DELIMS));
  }
  return labels;
}

/* TODO: needs new home; does not belong here */
struct StringStack *get_clcmds_from_clb(char *fname)
{
  struct DataBlock db, dbem;
  struct DoubleA *dd;
  struct StringStack *result;

  db = fileToDataBlock(fname);
  dd = load_DataBlock_package(db);
  dbem = scanForTag(dd, TAGNUM_COMMANDS);
  result = loadTaggedStringStack(dbem, 1, "COMMANDS", TAGNUM_COMMANDS);

  datablockFree(db);
  datablockFree(dbem);
  doubleaFree(dd);
  return result;
}
