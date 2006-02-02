#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <complearn/complearn.h>

//void doParseTest(struct DataBlock *db);

int main(int argc, char **argv)
{
  const char *startfname, *dmfname, *outfname;
  struct DotParseTree *dpt;
  struct TreeMolder *tb;
  struct DataBlock *db, *matdb, *dotdb;
  struct CLNodeSet *clns = NULL;
  struct GeneralConfig *cur = loadDefaultEnvironment();
  double score;
  if (argc != 4) {
    fprintf(stderr, "Usage: %s distmatrix.clb starttree.dot newtree.dot\n", argv[0]);
    exit(1);
  }
  dmfname = argv[1];
  startfname = argv[2];
  outfname = argv[3];
  fprintf(stderr, "Opening tree %s\n", startfname);
  /* All func */
  db = fileToDataBlockPtr(startfname);
  matdb = fileToDataBlockPtr(dmfname);
  dpt = parseDotDB(db, matdb);
  tb = treemolderNew(dpt->dm, dpt->tree);
  clns = treemolderFlips(tb);
  score = 0;
  dotdb = convertTreeToDot(dpt->tree, score, dpt->labels, clns, cur,
                           NULL, dpt->dm);
  fprintf(stderr, "Reordering finished:  writing tree %s\n", outfname);
  datablockWriteToFile(dotdb, outfname);
  datablockFreePtr(dotdb);
  dotdb = NULL;
  return 0;
}
