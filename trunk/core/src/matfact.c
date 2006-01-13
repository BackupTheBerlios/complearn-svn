#include <string.h>

#include <complearn/complearn.h>

struct GSLMHdr {
  int size1, size2;
};

#if GSL_RDY
gsl_matrix *gslmatrixClone(const gsl_matrix *a)
{
  gsl_matrix *u;
  u = gsl_matrix_alloc(a->size1, a->size2);
  gsl_matrix_memcpy(u, a);
  return u;
}
struct DataBlock *gslmatrixDump(const gsl_matrix *a)
{
  struct DataBlock *result, *dubs;
  struct DoubleA *dac = doubleaNew();
  struct TagHdr h;
  struct GSLMHdr m;
  unsigned char *dbptr;
  int dbsize;
  int x, y;
  for (x = 0; x < a->size1; ++x) {
    for (y = 0; y < a->size2; ++y) {
      union PCTypes p = zeropct;
      p.d = gsl_matrix_get(a, x, y);
      doubleaPush(dac, p);
    }
  }
  h.tagnum = TAGNUM_GSLMATRIX;
  m.size1 = a->size1;
  m.size2 = a->size2;
  dubs = doubleaDump(dac);
  h.size = datablockSize(dubs) + sizeof(m);

  dbsize = datablockSize(dubs) + sizeof(h) + sizeof(m);
  dbptr = clCalloc(dbsize, 1);
  memcpy(dbptr, &h, sizeof(h));
  memcpy(dbptr + sizeof(h), &m, sizeof(m));
  memcpy(dbptr + sizeof(h) + sizeof(m), datablockData(dubs), datablockSize(dubs));
  result = datablockNewFromBlock(dbptr, dbsize);
  datablockFreePtr(dubs);
  clFree(dbptr);
  doubleaFree(dac);
  return result;
}

gsl_matrix *gslmatrixLoad(struct DataBlock *ptrd, int fmustbe)
{
  gsl_matrix *result;
  struct TagHdr *h;
  struct GSLMHdr *m;
  struct DataBlock *db, *d;
  struct DoubleA *da;
  int x, y, i = 0;
  d = ptrd;
  assert(sizeof(*m)+sizeof(*h) <= datablockSize(d));
  h = (struct TagHdr *) datablockData(d);
  m = (struct GSLMHdr *) (datablockData(d) + sizeof(*h));
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
  db = datablockNewFromBlock( datablockData(d) + sizeof(*h) + sizeof(*m), datablockSize(d) - (sizeof(*h) + sizeof(*m)));

  da = doubleaLoad(db, 1);

  result = gsl_matrix_alloc(m->size1, m->size2);
  for (x = 0; x < m->size1; x += 1)
    for (y = 0; y < m->size2; y += 1)
      gsl_matrix_set(result, x, y, doubleaGetDValueAt(da, i++));
  doubleaFree(da);
  datablockFreePtr(db);
  return result;
}

struct DataBlock *distmatrixDump(gsl_matrix *m)
{
  struct DataBlock *db, *dbm;
  db = gslmatrixDump(m);
  dbm = package_DataBlocks(TAGNUM_CLDISTMATRIX,db,NULL);
  return dbm;
}

gsl_matrix *distmatrixLoad(struct DataBlock *ptrdb, int fmustbe)
{
  gsl_matrix *m;
  struct DoubleA *dd;
  struct TagHdr *h = (struct TagHdr *) datablockData(ptrdb);
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

  dd = load_DataBlock_package(ptrdb);
  dbdm = scanForTag(dd, TAGNUM_GSLMATRIX );
  m = gslmatrixLoad(dbdm, 1);
  doubleaFree(dd);
  datablockFreePtr(dbdm);

  return m;
}

void gslmatrixFree(gsl_matrix *m)
{
  gsl_matrix_free(m);
}

gsl_matrix *clbDBDistMatrix(struct DataBlock *db)
{
  struct DataBlock *dbdm;
  gsl_matrix *dm;
  struct DoubleA *dd;

  dd = load_DataBlock_package(db);
  dbdm = scanForTag(dd, TAGNUM_CLDISTMATRIX);
  dm = clbDistMatrixLoad(dbdm);

  datablockFreePtr(dbdm);
  doubleaFree(dd);
  return dm;
}

struct DataBlock *clbDMDataBlock(char *fname) {
  struct DataBlock *db, *dbdm;
  struct DoubleA *dd;

  db = fileToDataBlockPtr(fname);
  dd = load_DataBlock_package(db);
  dbdm = scanForTag(dd, TAGNUM_CLDISTMATRIX);

  datablockFreePtr(db);
  doubleaFree(dd);
  return dbdm;
}

gsl_matrix *clbDistMatrixLoad(struct DataBlock *db)
{
  gsl_matrix *dm;
  dm = distmatrixLoad(db,1);
  return dm;
}


gsl_matrix *clbDistMatrix(char *fname)
{
  struct DataBlock *db;
  gsl_matrix *result;

  db = clbDMDataBlock(fname);
  result = clbDistMatrixLoad(db);

  datablockFreePtr(db);
  return result;
}


#define DELIMS " ,\t\r\n"
#define MAXLINESIZE 1024

static struct DoubleA *get_dm_row_from_txt(char *linebuf, int isLabeled)
{
  struct DoubleA *row = doubleaNew();
  char *s;
  union PCTypes p = zeropct;
  s = strtok(linebuf, DELIMS);
  if (!isLabeled) {
    p.d = atof(s);
    doubleaPush(row,p);
  }
  while((s = strtok(NULL, DELIMS))) {
    p.d = atof(s);
    doubleaPush(row,p);
  }
  return row;
}

int cltxtRowSize(char *fname)
{
  FILE *fp;
  int rows = 0;
  char linebuf[MAXLINESIZE];
  fp = clfopen(fname, "r");
  while (fgets(linebuf, MAXLINESIZE, fp)) {
    rows += 1;
  }
  clfclose(fp);
  return rows;
}

int cltxtColSize(char *fname)
{
  FILE *fp;
  int cols = 0;
  char *s, linebuf[MAXLINESIZE];

  fp = clfopen(fname, "r");
  fgets(linebuf, MAXLINESIZE, fp);
  if ((s = strtok(linebuf, DELIMS)))
    cols += 1;
  while ((s = strtok(NULL, DELIMS))) {
    cols += 1;
  }
  clfclose(fp);
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
  fp = clfopen(fname, "r");


  for (i = 0; i < rows ; i += 1) {
    struct DoubleA *rowvals;
    fgets(linebuf, MAXLINESIZE, fp);
    rowvals = get_dm_row_from_txt(linebuf, isLabeled);
    for (j = 0; j < doubleaSize(rowvals); j +=1) {
      gsl_matrix_set(result, i, j, doubleaGetValueAt(rowvals, j).d);
    }
  }

  clfclose(fp);
  return result;
}

void gslmatrixPrint(gsl_matrix *m, char *delim)
{
  int i, j;
  for (i = 0; i < m->size1 ; i += 1) {
    for (j = 0; j < m->size2 ; j +=1) {
      printf("%f%s", gsl_matrix_get(m, i, j), (j<m->size2- 1) ? delim : "\n");
    }
  }
}
#endif
