#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <complearn/complearn.h>

//void doParseTest(struct DataBlock *db);

int main(int argc, char **argv)
{
  const char *fname;
  if (argv[1])
    fname = argv[1];
  else
    fname = "treefile.dot";
  fprintf(stderr, "Opening tree %s\n", fname);
  struct DataBlock *db = fileToDataBlockPtr(fname);
  doParseTest(db);
  return 0;
}
