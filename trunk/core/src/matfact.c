/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
    clLogError("Error: expecting GSLMATRIX tagnum %x, got %x\n",
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
  dbm = clPackageDataBlocks(TAGNUM_CLDISTMATRIX,db,NULL);
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
      clLogError("Error: expecting CLDISTMATRIX tagnum %x, got %x\n",
          TAGNUM_CLDISTMATRIX,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }

  dd = clLoadDatablockPackage(ptrdb);
  dbdm = clScanForTag(dd, TAGNUM_GSLMATRIX );
  m = clGslmatrixLoad(dbdm, 1);
  clFreeDataBlockpackage (dd);
  clDatablockFreePtr(dbdm);

  return m;
}

void clGslmatrixFree(gsl_matrix *m)
{
  gsl_matrix_free(m);
}

int clbIsCLBFile(struct DataBlock *db)
{
  struct TransformAdaptor *b;
  b = clBuiltin_UNBZIP();
  char smallbuf[200];
  int isbz2c = 0, result;
  struct DataBlock *curdb;
  if (clDatablockSize(db) < 4)
    return 0;
  memcpy(smallbuf, clDatablockData(db), 4);
  if (b && b->pf(db)) {
    isbz2c = 1;
    curdb = b->tf(db);
  }
  else
    curdb = clDatablockClonePtr(db);

  if (clDatablockSize(curdb) < 200) {
    clDatablockFreePtr(curdb);
    return 0;
  }
  memcpy(smallbuf, clDatablockData(curdb), 199);
  smallbuf[199] = 0;
  result = strstr(smallbuf, "<clb") ? 1 : 0;
  clDatablockFreePtr(curdb);
  return result;
}

struct StringStack *clReadAnyDistMatrixLabels(struct DataBlock *db)
{
  if (clbIsCLBFile(db))
    return clbDBLabels(db);
  else {
    struct StringStack *labels;
    labels = clStringstackNew();
    gsl_matrix *gm;
    gm = clTxtDistMatrix(db, labels);
    gsl_matrix_free(gm);
    return labels;
  }
}

gsl_matrix *clReadAnyDistMatrix(struct DataBlock *db)
{
  if (clbIsCLBFile(db))
    return clbDBDistMatrix(db);
  else {
    return clTxtDistMatrix(db, NULL);
    //printf("error not implemented 4 yet\n");
    //exit(1);
  }
}


struct DataBlock *clbDMDataBlock(char *fname) {
  struct DataBlock *db, *dbdm;
  struct DRA *dd;

  db = clFileToDataBlockPtr(fname);
  dd = clLoadDatablockPackage(db);
  dbdm = clScanForTag(dd, TAGNUM_CLDISTMATRIX);

  clDatablockFreePtr(db);
  clFreeDataBlockpackage(dd);
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
#define MAXLINESIZE 10240

static int grabFields(gsl_matrix *m, const char *rowStart, int row, struct StringStack *labels)
{
  char numbuf[2048];
  int numtop = 100;
  int numpos;
  const char *rowEnd = index(rowStart, '\n');
  if (rowEnd == NULL)
    rowEnd = rowStart + strlen(rowStart);
  numpos = numtop;
  numbuf[numtop+1] = 0;
  int colind = m->size1-1;
  while (colind >= 0 && rowEnd > rowStart) {
    int c;
    c = *rowEnd--;
    if ((c >= '0' && c <= '9') || c == '.') {
      numbuf[numpos--] = c;
    }
    else {
      if (numpos < numtop) {
        double v = atof(numbuf+numpos+1);
        gsl_matrix_set(m, colind--, row, v);
        numpos = numtop;
      }
    }
  }
  memset(numbuf, 0, sizeof(numbuf));
  strncpy(numbuf, rowStart, rowEnd-rowStart);
  if (labels)
    clStringstackPush(labels, numbuf);
  return 0;
}

int clTxtRowSize(struct DataBlock *db)
{
  int i;
  int sz = clDatablockSize(db);
  unsigned char *d = clDatablockData(db);
  int goodrow = 0;
  int c=0, lc;
  int rowCount = 0;
  for (i = 0; i < sz; i += 1) {
    lc = c;
    c = d[i];
    if (!(c == '\r' || c == '\n' || c == '\t' || c == ' '))
      goodrow = 1;
    if ((c == '\r' || c == '\n') && (lc != '\r' && lc != '\n')) {
      rowCount += goodrow;
      goodrow = 0;
    }
  }
  return rowCount;
}

struct StringStack *clDefaultLabels(int sz)
{
  int i;
  struct StringStack *ss = clStringstackNew();
  for (i = 0; i < sz; i += 1) {
    char buf[80];
    sprintf(buf, "item%02d", i+1);
    clStringstackPush(ss,buf);
  }
  return ss;
}

gsl_matrix *clTxtDistMatrix(struct DataBlock *db, struct StringStack *ulabels)
{
  int i;
  int sz = clDatablockSize(db);
  char *d = (char *) clDatablockData(db);
  int goodrow = 0;
  int c=0, lc;
  int curRow = 0;
  int rows = 0;
  char *lastRow;
  struct StringStack *labels = NULL;
  gsl_matrix *result;
  if (ulabels)
    labels = clStringstackNew();

  rows = clTxtRowSize(db);
  if (rows < 4) {
    fprintf(stderr, "Error, only %d rows in matrix, but need at least 4.\n", rows);
    exit(1);
  }
  result = gsl_matrix_alloc(rows,rows);
  lastRow = d;
  for (i = 0; i < sz; i += 1) {
    lc = c;
    c = d[i];
    if (!(c == '\r' || c == '\n' || c == '\t' || c == ' ')) {
      goodrow = 1;
      if (lc == '\r' || lc == '\n')
        lastRow = d + i;
    }
    if ((c == '\r' || c == '\n') && (lc != '\r' && lc != '\n')) {
      if (goodrow)
        grabFields(result, lastRow, curRow, labels);
      curRow += goodrow;
      goodrow = 0;
    }
  }
  if (labels) {
    if (clStringstackLongestLength(labels) == 0) {
      clStringstackFree(labels);
      labels = clDefaultLabels(sz);
    }
    for (i = 0; i < clStringstackSize(labels); i += 1)
      clStringstackPush(ulabels, clStringstackReadAt(labels, i));
    clStringstackFree(labels);
  }
  return result;
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

