#include <string.h>

#include <complearn/complearn.h>

struct GSLMHdr {
  int size1, size2;
};

gsl_matrix *clGslmatrixClone(const gsl_matrix *a)
{
  gsl_matrix *u;
  u = gsl_matrix_alloc(a->size1, a->size2);
  gsl_matrix_memcpy(u, a);
  return u;
}
struct DataBlock *clGslmatrixDump(const gsl_matrix *a)
{
  struct DataBlock *result, *dubs;
  struct DRA *dac = clDraNew();
  struct TagHdr h;
  struct GSLMHdr m;
  unsigned char *dbptr;
  int dbsize;
  int x, y;
  for (x = 0; x < a->size1; ++x) {
    for (y = 0; y < a->size2; ++y) {
      union PCTypes p = zeropct;
      p.d = gsl_matrix_get(a, x, y);
      clDraPush(dac, p);
    }
  }
  h.tagnum = TAGNUM_GSLMATRIX;
  m.size1 = a->size1;
  m.size2 = a->size2;
  dubs = clDraDump(dac);
  h.size = clDatablockSize(dubs) + sizeof(m);

  dbsize = clDatablockSize(dubs) + sizeof(h) + sizeof(m);
  dbptr = clCalloc(dbsize, 1);
  memcpy(dbptr, &h, sizeof(h));
  memcpy(dbptr + sizeof(h), &m, sizeof(m));
  memcpy(dbptr + sizeof(h) + sizeof(m), clDatablockData(dubs), clDatablockSize(dubs));
  result = clDatablockNewFromBlock(dbptr, dbsize);
  clDatablockFreePtr(dubs);
  clFree(dbptr);
  clDraFree(dac);
  return result;
}

gsl_matrix *clGslmatrixLoad(struct DataBlock *ptrd, int fmustbe)
{
  gsl_matrix *result;
  struct TagHdr *h;
  struct GSLMHdr *m;
  struct DataBlock *db, *d;
  struct DRA *da;
  int x, y, i = 0;
  d = ptrd;
  assert(sizeof(*m)+sizeof(*h) <= clDatablockSize(d));
  h = (struct TagHdr *) clDatablockData(d);
  m = (struct GSLMHdr *) (clDatablockData(d) + sizeof(*h));
  if (h->tagnum != TAGNUM_GSLMATRIX) {
    if (fmustbe) {
    clogError("Error: expecting GSLMATRIX tagnum %x, got %x\n",
        TAGNUM_GSLMATRIX,h->tagnum);
    exit(1);
    }
    else
      return NULL;
  }
  assert(m->size1 > 0 && m->size2 > 0);
  db = clDatablockNewFromBlock( clDatablockData(d) + sizeof(*h) + sizeof(*m), clDatablockSize(d) - (sizeof(*h) + sizeof(*m)));

  da = clDraLoad(db, 1);

  result = gsl_matrix_alloc(m->size1, m->size2);
  for (x = 0; x < m->size1; x += 1)
    for (y = 0; y < m->size2; y += 1)
      gsl_matrix_set(result, x, y, clDraGetDValueAt(da, i++));
  clDraFree(da);
  clDatablockFreePtr(db);
  return result;
}

struct DataBlock *clDistmatrixDump(gsl_matrix *m)
{
  struct DataBlock *db, *dbm;
  db = clGslmatrixDump(m);
  dbm = clPackage_DataBlocks(TAGNUM_CLDISTMATRIX,db,NULL);
  return dbm;
}

gsl_matrix *clDistmatrixLoad(struct DataBlock *ptrdb, int fmustbe)
{
  gsl_matrix *m;
  struct DRA *dd;
  struct TagHdr *h = (struct TagHdr *) clDatablockData(ptrdb);
  struct DataBlock *dbdm;

  if (h->tagnum != TAGNUM_CLDISTMATRIX) {
    if (fmustbe) {
      clogError("Error: expecting CLDISTMATRIX tagnum %x, got %x\n",
          TAGNUM_CLDISTMATRIX,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }

  dd = clLoad_DataBlock_package(ptrdb);
  dbdm = clScanForTag(dd, TAGNUM_GSLMATRIX );
  m = clGslmatrixLoad(dbdm, 1);
  clDraFree(dd);
  clDatablockFreePtr(dbdm);

  return m;
}

void clGslmatrixFree(gsl_matrix *m)
{
  gsl_matrix_free(m);
}

gsl_matrix *clbDBDistMatrix(struct DataBlock *db)
{
  struct DataBlock *dbdm;
  gsl_matrix *dm;
  struct DRA *dd;

  dd = clLoad_DataBlock_package(db);
  dbdm = clScanForTag(dd, TAGNUM_CLDISTMATRIX);
  dm = clbDistMatrixLoad(dbdm);

  clDatablockFreePtr(dbdm);
  clDraFree(dd);
  return dm;
}

struct DataBlock *clbDMDataBlock(char *fname) {
  struct DataBlock *db, *dbdm;
  struct DRA *dd;

  db = clFileToDataBlockPtr(fname);
  dd = clLoad_DataBlock_package(db);
  dbdm = clScanForTag(dd, TAGNUM_CLDISTMATRIX);

  clDatablockFreePtr(db);
  clDraFree(dd);
  return dbdm;
}

gsl_matrix *clbDistMatrixLoad(struct DataBlock *db)
{
  gsl_matrix *dm;
  dm = clDistmatrixLoad(db,1);
  return dm;
}


gsl_matrix *clbDistMatrix(char *fname)
{
  struct DataBlock *db;
  gsl_matrix *result;

  db = clbDMDataBlock(fname);
  result = clbDistMatrixLoad(db);

  clDatablockFreePtr(db);
  return result;
}


#define DELIMS " ,\t\r\n"
#define MAXLINESIZE 1024

static struct DRA *get_dm_row_from_txt(char *linebuf, int isLabeled)
{
  struct DRA *row = clDraNew();
  char *s;
  union PCTypes p = zeropct;
  s = strtok(linebuf, DELIMS);
  if (!isLabeled) {
    p.d = atof(s);
    clDraPush(row,p);
  }
  while((s = strtok(NULL, DELIMS))) {
    p.d = atof(s);
    clDraPush(row,p);
  }
  return row;
}

int cltxtRowSize(char *fname)
{
  FILE *fp;
  int rows = 0;
  char linebuf[MAXLINESIZE];
  fp = clFopen(fname, "r");
  while (fgets(linebuf, MAXLINESIZE, fp)) {
    rows += 1;
  }
  clFclose(fp);
  return rows;
}

int cltxtColSize(char *fname)
{
  FILE *fp;
  int cols = 0;
  char *s, linebuf[MAXLINESIZE];

  fp = clFopen(fname, "r");
  fgets(linebuf, MAXLINESIZE, fp);
  if ((s = strtok(linebuf, DELIMS)))
    cols += 1;
  while ((s = strtok(NULL, DELIMS))) {
    cols += 1;
  }
  clFclose(fp);
  return cols;
}

gsl_matrix *cltxtDistMatrix(char *fname)
{
  char linebuf[MAXLINESIZE];
  FILE *fp;
  int rows = 0;
  int cols = 0;
  int isLabeled = 0;
  int i, j;
  gsl_matrix *result;

  rows = cltxtRowSize(fname);
  cols = cltxtColSize(fname);

  if (cols > rows) isLabeled = 1;
  result = gsl_matrix_alloc(rows,rows);
  fp = clFopen(fname, "r");

  for (i = 0; i < rows ; i += 1) {
    struct DRA *rowvals;
    fgets(linebuf, MAXLINESIZE, fp);
    rowvals = get_dm_row_from_txt(linebuf, isLabeled);
    for (j = 0; j < clDraSize(rowvals); j +=1) {
      gsl_matrix_set(result, i, j, clDraGetValueAt(rowvals, j).d);
    }
  }

  clFclose(fp);
  return result;
}

int cltxtToCLB(char *source, char *dest)
{
  struct DataBlock *dmdb, *labelsdb, *clbdb;
  gsl_matrix *dm;
  struct StringStack *labels;

  dm = cltxtDistMatrix(source);
  labels = cltxtLabels(source);
  dmdb = clDistmatrixDump(dm);
  labelsdb = clLabelsDump(labels);

  clbdb = clPackage_DataBlocks(TAGNUM_TAGMASTER, dmdb, labelsdb, NULL);
  clDatablockWriteToFile(clbdb, dest);

  clDatablockFreePtr(clbdb);
  clDatablockFreePtr(dmdb);
  clDatablockFreePtr(labelsdb);
  clStringstackFree(labels);
  gsl_matrix_free(dm);

  return 1;
}

void clGslmatrixPrint(gsl_matrix *m, char *delim)
{
  int i, j;
  for (i = 0; i < m->size1 ; i += 1) {
    for (j = 0; j < m->size2 ; j +=1) {
      printf("%f%s", gsl_matrix_get(m, i, j), (j<m->size2- 1) ? delim : "\n");
    }
  }
}
