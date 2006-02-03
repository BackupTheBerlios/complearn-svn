#include <complearn/complearn.h>

int main(int argc, char **argv)
{
  struct DataBlock *db, *dbdm;
  struct DRA *dd;
  gsl_matrix *dm, *sdm;
  if (argc != 2) {
    printf("Usage: %s <distmatrix.cld>\n", argv[0]);
    exit(1);
  }
  db = clFileToDataBlockPtr(argv[1]);
  dd = clLoad_DataBlock_package(db);
  dbdm = clScanForTag(dd, TAGNUM_CLDISTMATRIX);
  dm = clDistmatrixLoad(dbdm, 1);
  clDatablockFreePtr(db);
  clDatablockFreePtr(dbdm);
  //printf("The matrix is %d by %d\n", dm->size1, dm->size2);
  sdm = clSvdProject(dm);
  clGslmatrixFree(dm);
  return 0;
}
