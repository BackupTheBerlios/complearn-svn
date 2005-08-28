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

struct IncrementalDistMatrix *incrdmNew(struct CompAdaptor *ca)
{
  struct IncrementalDistMatrix *idm;
  idm = clCalloc(sizeof(*idm), 1);
  idm->curmat = gsl_matrix_calloc(MAXDATABLOCK, MAXDATABLOCK);
  if (ca == NULL)
    ca = compaLoadBuiltin("blocksort");
  idm->ca = ca;
  return idm;
}

void incrdmAddDataBlock(struct IncrementalDistMatrix *idm, struct DataBlock *db)
{
  int i;
  int curguy = idm->dbcount;
  idm->dbcount += 1;
  idm->db[curguy] = datablockClonePtr(db);
  idm->singlesize[curguy] = compaCompress(idm->ca, *idm->db[curguy]);
  for (i = 0; i <= curguy; i += 1) {
    double bothsize, curcell;
    struct DataBlock combinedDataBlock;
    combinedDataBlock = datablockCat(*idm->db[curguy], *idm->db[i]);
    bothsize = compaCompress(idm->ca, combinedDataBlock);
    curcell = mndf(idm->singlesize[i], idm->singlesize[curguy], bothsize, bothsize);
    datablockFree(combinedDataBlock);
    gsl_matrix_set(idm->curmat, i, curguy, curcell);
    gsl_matrix_set(idm->curmat, curguy, i, curcell);
  }
}

void incrdmFree(struct IncrementalDistMatrix *idm)
{
  int i;
  gsl_matrix_free(idm->curmat);
  for (i = 0; i < idm->dbcount; i += 1) {
    datablockFreePtr(idm->db[i]);
    idm->db[i] = NULL;
  }
}

gsl_matrix *incrdmDistMatrix(struct IncrementalDistMatrix *idm)
{
  idm->result = gsl_matrix_submatrix(idm->curmat, 0, 0, idm->dbcount, idm->dbcount);
  return (gsl_matrix *) &idm->result;
}

int incrdmSize(struct IncrementalDistMatrix *idm)
{
  return idm->dbcount;
}

