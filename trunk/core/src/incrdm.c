#include <complearn.h>



struct IncrementalDistMatrix *clIncrdmNew(struct CompAdaptor *ca)
{
  struct IncrementalDistMatrix *idm;
  idm = clCalloc(sizeof(*idm), 1);
  idm->curmat = gsl_matrix_calloc(MAXDATABLOCK, MAXDATABLOCK);
  if (ca == NULL)
    ca = clCompaLoadBuiltin("blocksort");
  idm->ca = ca;
  return idm;
}

void clIncrdmAddDataBlock(struct IncrementalDistMatrix *idm, struct DataBlock *db)
{
  int i;
  int curguy = idm->dbcount;
  idm->dbcount += 1;
  idm->db[curguy] = clDatablockClonePtr(db);
  idm->singlesize[curguy] = clCompaCompress(idm->ca, idm->db[curguy]);
  for (i = 0; i <= curguy; i += 1) {
    double bothsize, curcell;
    struct DataBlock *combinedDataBlock;
    combinedDataBlock = clDatablockCatPtr(idm->db[curguy], idm->db[i]);
    bothsize = clCompaCompress(idm->ca, combinedDataBlock);
    curcell = clMndf(idm->singlesize[i], idm->singlesize[curguy], bothsize, bothsize);
    clDatablockFreePtr(combinedDataBlock);
    gsl_matrix_set(idm->curmat, i, curguy, curcell);
    gsl_matrix_set(idm->curmat, curguy, i, curcell);
  }
  idm->result = gsl_matrix_submatrix(idm->curmat, 0, 0, idm->dbcount, idm->dbcount);
}

void clIncrdmFree(struct IncrementalDistMatrix *idm)
{
  int i;
  gsl_matrix_free(idm->curmat);
  for (i = 0; i < idm->dbcount; i += 1) {
    clDatablockFreePtr(idm->db[i]);
    idm->db[i] = NULL;
  }
}

gsl_matrix *clIncrdmDistMatrix(struct IncrementalDistMatrix *idm)
{
  return (gsl_matrix *) &idm->result;
}

int clIncrdmSize(struct IncrementalDistMatrix *idm)
{
  return idm->dbcount;
}

