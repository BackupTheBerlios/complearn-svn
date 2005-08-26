#include <complearn.h>

#include <gsl/gsl_matrix.h>

#define MAXDATABLOCK 256

struct IncrementalDistMatrix {
  int dbcount;
  struct DataBlock *db[MAXDATABLOCK];
  double singlesize[MAXDATABLOCK];
  gsl_matrix *curmat;
  gsl_matrix_view result;
  struct CompAdaptor *ca;
};

struct IncrementalDistMatrix *newIDM(struct CompAdaptor *ca)
{
  struct IncrementalDistMatrix *idm;
  idm = gcalloc(sizeof(*idm), 1);
  idm->curmat = gsl_matrix_calloc(MAXDATABLOCK, MAXDATABLOCK);
  if (ca == NULL)
    ca = compaLoadBuiltin("blocksort");
  idm->ca = ca;
  return idm;
}

void addDataBlock(struct IncrementalDistMatrix *idm, struct DataBlock *db)
{
  int i;
  int curguy = idm->dbcount;
  idm->dbcount += 1;
  idm->db[curguy] = cloneDataBlockPtr(db);
  idm->singlesize[curguy] = compfuncCA(idm->ca, *idm->db[curguy]);
  for (i = 0; i <= curguy; i += 1) {
    double bothsize, curcell;
    struct DataBlock combinedDataBlock;
    combinedDataBlock = catDataBlock(*idm->db[curguy], *idm->db[i]);
    bothsize = compfuncCA(idm->ca, combinedDataBlock);
    curcell = mndf(idm->singlesize[i], idm->singlesize[curguy], bothsize, bothsize);
    freeDataBlock(combinedDataBlock);
    gsl_matrix_set(idm->curmat, i, curguy, curcell);
    gsl_matrix_set(idm->curmat, curguy, i, curcell);
  }
}

void freeIncrementalDistMatrix(struct IncrementalDistMatrix *idm)
{
  int i;
  gsl_matrix_free(idm->curmat);
  for (i = 0; i < idm->dbcount; i += 1) {
    freeDataBlockPtr(idm->db[i]);
    idm->db[i] = NULL;
  }
}

gsl_matrix *getDistMatrixIDM(struct IncrementalDistMatrix *idm)
{
  idm->result = gsl_matrix_submatrix(idm->curmat, 0, 0, idm->dbcount, idm->dbcount);
  return (gsl_matrix *) &idm->result;
}

int getSizeIDM(struct IncrementalDistMatrix *idm)
{
  return idm->dbcount;
}

