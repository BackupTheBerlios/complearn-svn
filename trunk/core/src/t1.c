#include <complearn/complearn.h>

int main()
{
  struct DataBlock *dbkey, *dbval, *fval;
  struct GDBMHelper *gdbm = cldbopen("fun");
  dbkey = clStringToDataBlockPtr("lisa");
  dbval = clStringToDataBlockPtr("gogo dancer");
  fval = cldbfetch(gdbm, dbkey);
  printf("The fval is %p\n", fval);
  cldbstore(gdbm, dbkey, dbval);
  printf("stored\n");
  fval = cldbfetch(gdbm, dbkey);
  printf("The fval is %p\n", fval);
  return 0;
}
