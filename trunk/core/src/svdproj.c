#include <complearn/complearn.h>

int main(int argc, char **argv)
{
  struct DataBlock *db, *dbdm;
  struct DoubleA *dd;
  gsl_matrix *dm, *sdm;
  if (argc != 2) {
    printf("Usage: %s <distmatrix.cld>\n", argv[0]);
    exit(1);
  }
  db = fileToDataBlockPtr(argv[1]);
  dd = load_DataBlock_package(db);
  dbdm = scanForTag(dd, TAGNUM_CLDISTMATRIX);
  dm = distmatrixLoad(dbdm, 1);
  datablockFreePtr(db);
  datablockFreePtr(dbdm);
  //printf("The matrix is %d by %d\n", dm->size1, dm->size2);
  sdm = svdProject(dm);
  gslmatrixFree(dm);
  return 0;
}
