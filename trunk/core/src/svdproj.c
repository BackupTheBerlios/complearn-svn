#include <complearn/complearn.h>

int main(int argc, char **argv)
{
#if GSL_RDY
  struct DataBlock db, dbdm;
  struct DoubleA *dd;
  gsl_matrix *dm, *sdm;
  if (argc != 2) {
    printf("Usage: %s <distmatrix.cld>\n", argv[0]);
    exit(1);
  }
  db = fileToDataBlock(argv[1]);
  dd = load_DataBlock_package(db);
  dbdm = scanForTag(dd, TAGNUM_CLDISTMATRIX);
  dm = distmatrixLoad(dbdm, 1);
  datablockFree(db);
  datablockFree(dbdm);
  //printf("The matrix is %d by %d\n", dm->size1, dm->size2);
  sdm = svdProject(dm);
  gslmatrixFree(dm);
  return 0;
#else
	printf("no gsl support; svd projection impossible\n");
	exit(1);
	return 1;
#endif
}
