#include <complearn/complearn.h>

// looking at http://cran.r-project.org/doc/manuals/R-exts.html
// and        http://www.sfu.ca/~sblay/R-C-interface.txt
//

int main(int argc, char **argv)
{
  double result;
  struct DataBlock *db1, *db2;
  struct CompressionBase *comp = NULL;
  if (comp == NULL) {
    comp = clNewCompressorCB("blocksort");
  }
  printf("About A\n");
  db1 = clStringToDataBlockPtr( "hello");
  printf("About B\n");
  db2 = clStringToDataBlockPtr( "segfault");
  printf("About C with comp %p\n", comp);
// TODO: figure out if this is a dynamic-linking bug in Makefile or what
  result = clNcdFuncCB(comp, db1, db2);
  printf("About D\n");
  clDatablockFreePtr(db1);
  clDatablockFreePtr(db2);
  result = 0.6;
  return 0;
}
