#include <complearn/complearn.h>

int main(int argc, char **argv)
{
#if GSL_RDY
  struct DataBlock db, dbdm;
  struct DoubleA *dd;
  gsl_matrix *dm;
  if (argc != 2) {
    printf("Usage: %s <distmatrix.cld>\n", argv[0]);
    exit(1);
  }
  db = convertFileToDataBlock(argv[1]);
  dd = load_DataBlock_package(db);
  dbdm = scanForTag(dd, TAGNUM_CLDISTMATRIX);
  dm = loadCLDistMatrix(dbdm, 1);
  freeDataBlock(db);
  freeDataBlock(dbdm);
  printf("The matrix is %d by %d\n", dm->size1, dm->size2);
  freeGSLMatrix(dm);
  return 0;
#else
	printf("no gsl support; svd projection impossible\n");
	exit(1);
	return 1;
#endif
}
