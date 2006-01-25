#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <complearn/complearn.h>

//void doParseTest(struct DataBlock *db);

int main(int argc, char **argv)
{
  const char *fname, *dmfname;
  struct DotParseTree *dpt;
  struct TreeMolder *tb;
  struct DataBlock *db, *matdb, *dotdb;
  struct CLNodeSet *clns = NULL;
  struct GeneralConfig *cur = loadDefaultEnvironment();
  double score;
  if (argv[1])
    fname = argv[1];
  else
    fname = "treefile.dot";
  if (argc > 3)
    dmfname = argv[2];
  else
    dmfname = "distmatrix.clb";
  fprintf(stderr, "Opening tree %s\n", fname);
  /* All func */
  db = fileToDataBlockPtr(fname);
  matdb = fileToDataBlockPtr(dmfname);
  dpt = parseDotDB(db, matdb);
  tb = treemolderNew(dpt->dm, dpt->tree);
  clns = treemolderFlips(tb);
  score = 0;
  dotdb = convertTreeToDot(dpt->tree, score, dpt->labels, clns, cur,
                           NULL, dpt->dm);
  datablockWriteToFile(dotdb, "newtree.dot");
  datablockFreePtr(dotdb);
  dotdb = NULL;
  return 0;
}
